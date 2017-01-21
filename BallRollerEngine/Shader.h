#pragma once

#include "GLutils.h"
#include <string>

class CShader {
private:
  glm::uint32  mId;

public:
  CShader(const glm::uint32 type);
  ~CShader();

  const bool Compile(const std::string& source);
  const bool IsCompiled() const;
  const std::string GetLog() const;

  const glm::uint32 GetId() const;
};

class CShaderProgram {
private:
  glm::uint32  mId;

public:
  CShaderProgram();
  ~CShaderProgram();

  void Attach(const CShader& shader);

  const bool Link();
  const bool IsLinked() const;
  const std::string GetLog() const;

  void Bind() const;
  void Unbind() const;

  const glm::int32 GetAttributeIndex(const std::string& name) const;
  const glm::int32 GetUniformIndex(const std::string& name) const;
};

extern CShaderProgram* CreateShaderProgram(const std::string& vertSource, const std::string& fragSource, std::string& outLog);

