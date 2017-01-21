#include "stdafx.h"
#include "Shader.h"

CShader::CShader(const glm::uint32 type) 
  : mId(0) 
{
  mId = glCreateShader(type);
}

CShader::~CShader() {
  glDeleteShader(mId);
}

const bool CShader::Compile(const std::string & source) {
  const GLchar* szSource = reinterpret_cast<const GLchar*>(source.c_str());
  glShaderSource(mId, 1, &szSource, nullptr);

  glCompileShader(mId);

  return IsCompiled();
}

const bool CShader::IsCompiled() const {
  GLint status;
  glGetShaderiv(mId, GL_COMPILE_STATUS, &status);

  return status == GL_TRUE;
}

const std::string CShader::GetLog() const {
  GLint len;
  glGetShaderiv(mId, GL_INFO_LOG_LENGTH, &len);
  if(len > 0) {
    std::string log;
    log.resize(len - 1);
    glGetShaderInfoLog(mId, len, NULL, &log[0]);
    return log;
  }
  return std::string();
}

const glm::uint32 CShader::GetId() const {
  return mId;
}

CShaderProgram::CShaderProgram() : mId(0) {
  mId = glCreateProgram();
}

CShaderProgram::~CShaderProgram() {
  glDeleteProgram(mId);
}

void CShaderProgram::Attach(const CShader & shader) {
  glAttachShader(mId, shader.GetId());
}

const bool CShaderProgram::Link() {
  glLinkProgram(mId);
  return IsLinked();
}

const bool CShaderProgram::IsLinked() const {
  GLint status;
  glGetProgramiv(mId, GL_LINK_STATUS, &status);
  return status == GL_TRUE;
}

const std::string CShaderProgram::GetLog() const {
  GLint len;
  glGetProgramiv(mId, GL_INFO_LOG_LENGTH, &len);
  if(len > 0) {
    std::string log;
    log.resize(len - 1);
    glGetProgramInfoLog(mId, len, NULL, &log[0]);
    return log;
  }
  return std::string();
}

void CShaderProgram::Bind() const {
  glUseProgram(mId);
}

void CShaderProgram::Unbind() const {
  glUseProgram(0);
}

const glm::int32 CShaderProgram::GetAttributeIndex(const std::string & name) const {
  return glGetAttribLocation(mId, name.c_str());
}

const glm::int32 CShaderProgram::GetUniformIndex(const std::string & name) const {
  return glGetUniformLocation(mId, name.c_str());
}

CShaderProgram * CreateShaderProgram(const std::string & vertSource, const std::string & fragSource, std::string& outLog) {
  CShader vertShader(GL_VERTEX_SHADER);
  if(!vertShader.Compile(vertSource)) {
    outLog = vertShader.GetLog();
    return nullptr;
  }

  CShader fragShader(GL_FRAGMENT_SHADER);
  if(!fragShader.Compile(fragSource)) {
    outLog = fragShader.GetLog();
    return nullptr;
  }

  CShaderProgram* pProgram = new CShaderProgram();
  pProgram->Attach(vertShader);
  pProgram->Attach(fragShader);

  if(!pProgram->Link()) {
    outLog = pProgram->GetLog();
    delete pProgram;
    return nullptr;
  }

  return pProgram;
}
