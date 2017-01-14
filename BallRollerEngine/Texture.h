#pragma once

#include <GLES2/gl2.h>
#include <glm/glm.hpp>
#include <vector>

#include "GLHelper.h"

class CTexture {
private:
  GLuint mId;
  GLuint mTarget;
  GLuint mFormat;
  glm::uvec2 mSize;

public:
  CTexture(const GLenum format = GL_RGB565, const glm::uvec2 size = glm::uvec2(64));
  ~CTexture();

  void Bind(const GLuint unit = 0) const;
  void Unbind(const GLuint unit = 0) const;

  void Load(const GLenum inputFormat, GLenum dataFormat, const void* pData);
  void Load(const glm::uvec2 size, const GLenum format, const GLenum inputFormat, const GLenum dataFormat, const void* pData);

  const bool LoadPng(const std::vector<unsigned char>& pngData);

  template<typename _Type>
  void Load(const GLenum inputFormat, const std::vector<_Type>& data, const GLenum dataFormat = GetGLType<_Type>()) {
    Load(inputFormat, dataFormat, data.empty() ? nullptr : &data[0]);
  }
  template<typename _Type>
  void Load(const glm::uvec2 size, const GLenum format, const GLenum inputFormat, const std::vector<_Type>& data, const GLenum dataFormat = GetGLType<_Type>()) {
    Load(size, format, inputFormat, dataFormat, data.empty() ? nullptr : &data[0]);
  }

private:
  void ApplyParams();
};