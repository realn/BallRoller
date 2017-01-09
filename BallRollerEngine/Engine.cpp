#include "stdafx.h"
#include "Engine.h"
#include "Device.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

CEngine::CEngine(IDevice* pDevice) : mDevice(pDevice) {}

CEngine::~CEngine() {}

bool CEngine::Initialize() {

  mView = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));

  const std::string vertSource = SHADER_SOURCE(
    uniform highp mat4 mTransform;
    attribute highp vec3 vInVert;
    attribute highp vec4 vInColor;
    varying highp vec4 vPassColor;
    void main() {
      gl_Position = mTransform * vec4(vInVert, 1.0);
      vPassColor = vInColor;
    }
  );

  const std::string fragSource = SHADER_SOURCE(
    varying highp vec4 vPassColor;
    void main() {
      gl_FragColor = vPassColor;
    }
  );

  GLint vertShader = CreateShader(GL_VERTEX_SHADER, vertSource);
  GLint fragShader = CreateShader(GL_FRAGMENT_SHADER, fragSource);

  {
    std::vector<GLint> shaders;
    shaders.push_back(vertShader);
    shaders.push_back(fragShader);

    mShaderProgram = CreateShaderProgram(shaders);
  }

  glDeleteShader(vertShader);
  glDeleteShader(fragShader);

  mAttrVert = glGetAttribLocation(mShaderProgram, "vInVert");
  mAttrColor = glGetAttribLocation(mShaderProgram, "vInColor");

  mUniTransform = glGetUniformLocation(mShaderProgram, "mTransform");

  return true;
}

void CEngine::Release() {
  glDeleteProgram(mShaderProgram);
}

void CEngine::ScreenChanged(int width, int height) {
  float halfW = (float)width / 2.0f;
  float halfH = (float)height / 2.0f;

  mProj = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 10.0f);
}

void CEngine::FrameUpdate(const float timeDelta) {
  Update(timeDelta);
  Render();
}

void CEngine::Update(const float timeDelta) {}

void CEngine::Render() {
  glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
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
      log.resize(len-1);
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
