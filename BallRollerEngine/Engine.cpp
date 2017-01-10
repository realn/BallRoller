#include "stdafx.h"
#include "Engine.h"
#include "Device.h"

#include <cstdarg>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


CEngine::CEngine(IDevice* pDevice) :
  mDevice(pDevice), mRotation(0.0f) {}

CEngine::~CEngine() {}

bool CEngine::Initialize() {

  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);

  mView = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));

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

  glGenBuffers(1, &mVertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
  {
    struct Vertex {
      glm::vec3 mPos;
      glm::vec4 mColor;

      Vertex(const glm::vec3& pos, const glm::vec4& color) :
        mPos(pos), mColor(color) {}
    };

    std::vector<Vertex> verts;
    verts.push_back(Vertex(glm::vec3(0.0f, 0.7f, -0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
    verts.push_back(Vertex(glm::vec3(-0.5f, -0.3f, -0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)));
    verts.push_back(Vertex(glm::vec3(0.5f, -0.3f, -0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));

    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), &verts[0], GL_STATIC_DRAW);
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  return true;
}

void CEngine::Release() {
  glDeleteProgram(mShaderProgram);
  glDeleteBuffers(1, &mVertexBuffer);
}

void CEngine::ScreenChanged(int width, int height) {
  float asp = (float)width / (float)height;
  float halfW = asp;
  float halfH = 1.0f;

  mProj = glm::ortho(-halfW, halfW, -halfH, halfH);
  glViewport(0, 0, width, height);
}

void CEngine::FrameUpdate(const float timeDelta) {
  Update(timeDelta);
  Render();
}

void CEngine::Update(const float timeDelta) {
  mRotation += timeDelta * 30.0f;
  mRotation = glm::mod(mRotation, 360.0f);
  mView = glm::rotate(glm::mat4(1.0f), glm::radians(mRotation), glm::vec3(0.0f, 0.0f, 1.0f));
}

void CEngine::Render() {
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f); 
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(mShaderProgram);
  glm::mat4 matTrans = mProj * mView;
  glUniformMatrix4fv(mUniTransform, 1, GL_FALSE, glm::value_ptr(matTrans));

  GLuint vertexSize = sizeof(float) * 3 + sizeof(float) * 4;
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
  glVertexAttribPointer(mAttrPos, 3, GL_FLOAT, GL_FALSE, vertexSize, 0);
  glVertexAttribPointer(mAttrColor, 4, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<const void*>(sizeof(float) * 3));

  glEnableVertexAttribArray(mAttrPos);
  glEnableVertexAttribArray(mAttrColor);

  glDrawArrays(GL_TRIANGLES, 0, 3);

  glDisableVertexAttribArray(mAttrPos);
  glDisableVertexAttribArray(mAttrColor);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
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
