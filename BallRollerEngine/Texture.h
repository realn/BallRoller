#pragma once

#include "GLutils.h"
#include <vector>

class CTexture {
private:
  glm::uint32 mId;
  glm::uint32 mTarget;
  glm::uint32 mFormat;
  glm::uvec2 mSize;

public:
  CTexture(const glm::uint32 format = GL_RGB565, const glm::uvec2 size = glm::uvec2(64));
  ~CTexture();

  void Bind(const glm::uint32 unit = 0) const;
  void Unbind(const glm::uint32 unit = 0) const;

  void Load(const glm::uint32 inputFormat, glm::uint32 dataFormat, const void* pData);
  void Load(const glm::uvec2 size, const glm::uint32 format, const glm::uint32 inputFormat, const glm::uint32 dataFormat, const void* pData);

  const bool LoadPng(const std::vector<glm::uint8>& pngData);

  template<typename _Type>
  void Load(const glm::uint32 inputFormat, const std::vector<_Type>& data, const glm::uint32 dataFormat = GetGLType<_Type>()) {
    Load(inputFormat, dataFormat, data.empty() ? nullptr : &data[0]);
  }
  template<typename _Type>
  void Load(const glm::uvec2 size, const glm::uint32 format, const glm::uint32 inputFormat, const std::vector<_Type>& data, const glm::uint32 dataFormat = GetGLType<_Type>()) {
    Load(size, format, inputFormat, dataFormat, data.empty() ? nullptr : &data[0]);
  }

private:
  void ApplyParams();
};