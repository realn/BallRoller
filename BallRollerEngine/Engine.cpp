#include "stdafx.h"
#include "Engine.h"
#include "VertexDefinition.h"
#include "Mesh.h"
#include "MeshBuffer.h"
#include "MeshHelper.h"
#include "Texture.h"
#include "Shader.h"
#include "Font.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex {
  glm::vec3 mPos;
  glm::vec2 mCoord;

  Vertex(const glm::vec3& pos, const glm::vec2& coord) :
    mPos(pos), mCoord(coord) {}

  const bool operator==(const Vertex& v) const { return mPos == v.mPos && mCoord == v.mCoord; }
};

template<>
const Vertex CMeshCubeBuilder<Vertex>::MakeVertex(const glm::vec3 & pos, 
                                            const glm::vec3 & pass, 
                                            const glm::vec3 & offset, 
                                            const glm::vec3 & sideNormal, 
                                            const CMeshCubeBuilderBase::Side side) {
  glm::vec2 coord = glm::clamp(glm::sign(glm::vec2(pass)), 0.0f, 1.0f);
  return Vertex(pos, coord);
}


IEngine* IEngine::Create(IDevice* pDevice) {
  return new CEngine(pDevice);
}

CEngine::CEngine(IDevice* pDevice) :
  mDevice(pDevice), mRotation(0.0f), mpMesh(nullptr), mTexture(nullptr), mProgram(nullptr) {}

CEngine::~CEngine() {
}

const bool CEngine::Initialize() {
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  mView = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

  const std::string vertSource = SHADER_SOURCE(
    precision mediump float;
    uniform mat4 mTransform;
    attribute vec3 vInPos;
    attribute vec2 vInCoord;
    varying vec2 vPassCoord;
    void main() {
      gl_Position = mTransform * vec4(vInPos, 1.0);
      vPassCoord = vInCoord;
    }
  );

  const std::string fragSource = SHADER_SOURCE(
    precision mediump float;
    uniform sampler2D texBase;
    varying vec2 vPassCoord;
    void main() {
      vec4 color = texture2D(texBase, vPassCoord);
      color.a = 1.0;
      //gl_FragColor = vec4(1.0, 0.0, 1.0, 0.0);
      gl_FragColor = color;
    }
  );

  CShader vertShader(GL_VERTEX_SHADER);
  CShader fragShader(GL_FRAGMENT_SHADER);

  vertShader.Compile(vertSource);
  fragShader.Compile(fragSource);

  mProgram = new CShaderProgram();
  mProgram->Attach(vertShader);
  mProgram->Attach(fragShader);

  mProgram->Link();

  GLint attrPos = mProgram->GetAttributeIndex("vInPos");
  GLint attrCoord = mProgram->GetAttributeIndex("vInCoord");

  mUniTransform = mProgram->GetUniformIndex("mTransform");
  mUniTexBase = mProgram->GetUniformIndex("texBase");

  {
    std::vector<unsigned char> pngData;
    mDevice->LoadAsset("crate.png", pngData);

    mTexture = new CTexture();
    mTexture->LoadPng(pngData);
  }

  {
    CVertexDefinition def(sizeof(Vertex));
    def.AddStream(attrPos, 3, GL_FLOAT, 0);
    def.AddStream(attrCoord, 2, GL_FLOAT, 12);

    CMeshCubeBuilder<Vertex> meshBuilder;
    meshBuilder.BuildCube(1.0f);

    CMeshBuffer<Vertex>* meshBuf = meshBuilder.GetMesh();

    mpMesh = new CMesh(def);
    mpMesh->setVertices(meshBuf->GetVertices());
    mpMesh->setIndices(meshBuf->GetIndices());    
  }

  mFont = new CFont(mDevice);

  mText = new CFontText(mFont);
  mText->SetText(L"Marek to ja");

  return true;
}

void CEngine::Release() {
  delete mText;
  delete mFont;
  delete mProgram;
  delete mpMesh;
  delete mTexture;
}

void CEngine::ScreenChanged(const glm::int32 width, const glm::int32 height) {
  glm::float32 asp = (glm::float32)width / (glm::float32)height;
  glm::float32 halfW = (2.0f * asp) / 2.0f;
  glm::float32 halfH = 1.0f;

  //mProj = glm::ortho(-halfW, halfW, -halfH, halfH);
  mProj = glm::perspective(glm::radians(60.0f), asp, 1.0f, 100.0f);
  glViewport(0, 0, width, height);
}

void CEngine::FrameUpdate(const glm::float32 timeDelta) {
  Update(timeDelta);
  Render();
}

void CEngine::UserAction(UserActionType type) {
  const glm::float32 maxVel = 500.0f;
  if(type == UserActionType::MoveLeft) {
    mRotVel = -maxVel;
  }
  if(type == UserActionType::MoveRight) {
    mRotVel = maxVel;
  }
}

void CEngine::Update(const glm::float32 timeDelta) {

  mRotation += timeDelta * mRotVel;
  mRotation = glm::mod(mRotation, 360.0f);

  mRotVel -= (mRotVel * 0.3f) * timeDelta;
  if(glm::abs(mRotVel) < 0.01f)
    mRotVel = 0.0f;

  mView = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f)) * 
    glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
    glm::rotate(glm::mat4(1.0f), glm::radians(mRotation), glm::vec3(0.0f, 1.0f, 0.0f));
}

void CEngine::Render() {
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  glClearDepthf(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  {
    CBind<CShaderProgram> prog(mProgram);
    CBind<CTexture> tex(mTexture);
    CBind<CMesh> mesh(mpMesh);

    glm::mat4 matTrans = mProj * mView;
    glUniformMatrix4fv(mUniTransform, 1, GL_FALSE, glm::value_ptr(matTrans));
    glUniform1i(mUniTexBase, 0);

    mpMesh->Render();
  }

  {
    CBind<CFont> font(mFont);
    CGLState depth(GL_DEPTH_TEST, false);
    CGLState blend(GL_BLEND);
    CGLState cull(GL_CULL_FACE);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    mText->Render(glm::ortho(0.0f, 7.0f, 0.0f, 10.0f));
  }
}
