#include "stdafx.h"
#include "Engine.h"
#include "Device.h"
#include "VertexDefinition.h"
#include "Mesh.h"
#include "MeshBuffer.h"

#include <cstdarg>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


CEngine::CEngine(IDevice* pDevice) :
  mDevice(pDevice), mRotation(0.0f) {}

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
    attribute vec4 vInColor;
    varying vec4 vPassColor;
    void main() {
      gl_Position = mTransform * vec4(vInPos, 1.0);
      vPassColor = vInColor;
    }
  );

  const std::string fragSource = SHADER_SOURCE(
    precision mediump float;
    varying vec4 vPassColor;
    void main() {
      gl_FragColor = vPassColor;
    }
  );

  GLint vertShader = CreateShader(GL_VERTEX_SHADER, vertSource);
  GLint fragShader = CreateShader(GL_FRAGMENT_SHADER, fragSource);

  mShaderProgram = CreateShaderProgram(2, vertShader, fragShader);

  glDeleteShader(vertShader);
  glDeleteShader(fragShader);

  mAttrPos = glGetAttribLocation(mShaderProgram, "vInPos");
  mAttrColor = glGetAttribLocation(mShaderProgram, "vInColor");

  mUniTransform = glGetUniformLocation(mShaderProgram, "mTransform");

  {
    struct Vertex {
      glm::vec3 mPos;
      glm::vec4 mColor;

      Vertex(const glm::vec3& pos, const glm::vec4& color) :
        mPos(pos), mColor(color) {}

      const bool operator==(const Vertex& v) const { return mPos == v.mPos && mColor == v.mColor; }
    };

    CVertexDefinition def(sizeof(Vertex));
    def.AddStream(mAttrPos, 3, GL_FLOAT, 0);
    def.AddStream(mAttrColor, 4, GL_FLOAT, 12);

    glm::vec4 colRed(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 colGreen(0.0f, 1.0f, 0.0f, 1.0f);
    glm::vec4 colBlue(0.0f, 0.0f, 1.0f, 1.0f);

    float size = 1.0f;
    float half = size / 2.0f;
    Vertex v1(glm::vec3(-half, -half, half), colRed);
    Vertex v2(glm::vec3(half, -half, half), colGreen);
    Vertex v3(glm::vec3(half, half, half), colRed);
    Vertex v4(glm::vec3(-half, half, half), colBlue);
    Vertex v1b(glm::vec3(-half, -half, -half), colRed);
    Vertex v2b(glm::vec3(half, -half, -half), colGreen);
    Vertex v3b(glm::vec3(half, half, -half), colRed);
    Vertex v4b(glm::vec3(-half, half, -half), colBlue);

    CMeshBuffer<Vertex> meshBuf;
    meshBuf.AddQuadCCW(v1, v2, v3, v4);
    meshBuf.AddQuadCCW(v4b, v3b, v2b, v1b);

    meshBuf.AddQuadCCW(v2, v1, v1b, v2b);
    meshBuf.AddQuadCCW(v4, v3, v3b, v4b);
    meshBuf.AddQuadCCW(v1b, v1, v4, v4b);
    meshBuf.AddQuadCCW(v2, v2b, v3b, v3);

    mpMesh = new CMesh(def);
    mpMesh->setVertices(meshBuf.GetVertices());
    mpMesh->setIndices(meshBuf.GetIndices());
  }


  return true;
}

void CEngine::Release() {
  glDeleteProgram(mShaderProgram);
  delete mpMesh;
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

void CEngine::Update(const float timeDelta) {
  mRotation += timeDelta * 30.0f;
  mRotation = glm::mod(mRotation, 360.0f);
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

  mpMesh->Bind();
  mpMesh->Render();
  mpMesh->Unbind();

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
