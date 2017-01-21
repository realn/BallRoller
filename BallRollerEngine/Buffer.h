#pragma once

#include "GLutils.h"
#include <vector>

class CBuffer {
private:
  GLuint mId;
  GLenum mTarget;
  GLenum mUsage;

public:
  CBuffer(const GLenum target = GL_ARRAY_BUFFER, const GLenum usage = GL_STATIC_DRAW, const GLsizeiptr size = 0);
  ~CBuffer();

  void Bind() const;
  void Unbind() const;

  void Load(const void* pData, const GLsizeiptr size, const bool recreate = false);
  template<typename _Type>
  void Load(const std::vector<_Type>& data, const bool recreate = false) {
    if(!data.empty())
      Load(&data[0], data.size() * sizeof(_Type), recreate);
    else
      Load(nullptr, 0, recreate);
  }
};