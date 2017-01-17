#include "stdafx.h"
#include "Engine.h"
#include "Device.h"
#include "VertexDefinition.h"
#include "Mesh.h"
#include "MeshBuffer.h"
#include "MeshHelper.h"
#include "Texture.h"

#include <cstdarg>

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


CEngine::CEngine(IDevice* pDevice) :
  mDevice(pDevice), mRotation(0.0f), mpMesh(nullptr), mTexture(nullptr) {}

CEngine::~CEngine() {}

bool CEngine::Initialize() {

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

  GLint vertShader = CreateShader(GL_VERTEX_SHADER, vertSource);
  GLint fragShader = CreateShader(GL_FRAGMENT_SHADER, fragSource);

  mShaderProgram = CreateShaderProgram(2, vertShader, fragShader);

  glDeleteShader(vertShader);
  glDeleteShader(fragShader);

  mAttrPos = glGetAttribLocation(mShaderProgram, "vInPos");
  mAttrCoord = glGetAttribLocation(mShaderProgram, "vInCoord");

  mUniTransform = glGetUniformLocation(mShaderProgram, "mTransform");
  mUniTexBase = glGetUniformLocation(mShaderProgram, "texBase");

  {
    std::vector<unsigned char> pngData;
    mDevice->LoadAsset("crate.png", pngData);

    mTexture = new CTexture();
    mTexture->LoadPng(pngData);
    GLcheck("Texture");
  }

  {

    CVertexDefinition def(sizeof(Vertex));
    def.AddStream(mAttrPos, 3, GL_FLOAT, 0);
    def.AddStream(mAttrCoord, 2, GL_FLOAT, 12);

    CMeshCubeBuilder<Vertex> meshBuilder;
    meshBuilder.BuildCube(1.0f);

    CMeshBuffer<Vertex>* meshBuf = meshBuilder.GetMesh();

    mpMesh = new CMesh(def);
    mpMesh->setVertices(meshBuf->GetVertices());
    mpMesh->setIndices(meshBuf->GetIndices());    
  }

  return true;
}

void CEngine::Release() {
  glDeleteProgram(mShaderProgram);
  delete mpMesh;
  delete mTexture;
}

void CEngine::ScreenChanged(int width, int height) {
  float asp = (float)width / (float)height;
  float halfW = (2.0f * asp) / 2.0f;
  float halfH = 1.0f;

  //mProj = glm::ortho(-halfW, halfW, -halfH, halfH);
  mProj = glm::perspective(glm::radians(60.0f), asp, 1.0f, 100.0f);
  glViewport(0, 0, width, height);
}

void CEngine::FrameUpdate(const float timeDelta) {
  Update(timeDelta);
  Render();
}

void CEngine::UserAction(UserActionType type) {
  const float maxVel = 500.0f;
  if(type == UserActionType::MoveLeft) {
    mRotVel = -maxVel;
  }
  if(type == UserActionType::MoveRight) {
    mRotVel = maxVel;
  }
}

void CEngine::Update(const float timeDelta) {

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

  glUseProgram(mShaderProgram);
  glm::mat4 matTrans = mProj * mView;
  glUniformMatrix4fv(mUniTransform, 1, GL_FALSE, glm::value_ptr(matTrans));

  mTexture->Bind();
  glUniform1i(mUniTexBase, 0);

  mpMesh->Bind();
  mpMesh->Render();
  mpMesh->Unbind();

  mTexture->Unbind();
  glUseProgram(0);
}

GLint CEngine::CreateShader(const GLenum type, const std::string & source) {
  GLint shader = glCreateShader(type);

  const GLchar* szSource = reinterpret_cast<const GLchar*>(source.c_str());

  glShaderSource(shader, 1, &szSource, NULL);
  glCompileShader(shader);

  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if(status != GL_TRUE) {
    mDevice->Log("Shader Compilation Failed.");
    GLint len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    if(len > 0) {
      std::string log;
      log.resize(len - 1);
      glGetShaderInfoLog(shader, len, NULL, &log[0]);
      mDevice->Log(log);
    }

    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

GLint CEngine::CreateShaderProgram(const std::vector<GLint>& shaders) {
  GLint program = glCreateProgram();

  for(std::vector<GLint>::const_iterator it = shaders.begin(); it != shaders.end(); it++) {
    glAttachShader(program, *it);
  }

  glLinkProgram(program);

  GLint status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if(status != GL_TRUE) {
    mDevice->Log("Shader Program Linking Failed.");
    GLint len;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
    if(len > 0) {
      std::string log;
      log.resize(len - 1);
      glGetProgramInfoLog(program, len, NULL, &log[0]);
      mDevice->Log(log);
    }

    glDeleteProgram(program);
    return 0;
  }

  return program;
}

GLint CEngine::CreateShaderProgram(const int numberOfShaders, ...) {
  std::vector<GLint> shaders;
  va_list pt;
  va_start(pt, numberOfShaders);
  for(int i = 0; i < numberOfShaders; i++) {
    shaders.push_back(va_arg(pt, GLint));
  }
  va_end(pt);
  return CreateShaderProgram(shaders);
}

void CEngine::GLcheck(const std::string& func) {
  GLenum err = glGetError();
  if(err != GL_NO_ERROR) {
    mDevice->Log("GL call failed in func: " + func);
  }
}
