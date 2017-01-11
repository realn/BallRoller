#include "stdafx.h"
#include "Buffer.h"

CBuffer::CBuffer(const GLenum target, const GLenum usage, const GLsizeiptr size)
  : mId(0), mTarget(target), mUsage(usage) 
{
  glGenBuffers(1, &mId);
  glBindBuffer(mTarget, mId);
  glBufferData(mTarget, size, nullptr, mUsage);
  glBindBuffer(mTarget, 0);
}

CBuffer::~CBuffer() {
  glDeleteBuffers(1, &mId);
}

void CBuffer::Bind() const {
  glBindBuffer(mTarget, mId);
}

void CBuffer::Unbind() const {
  glBindBuffer(mTarget, 0);
}

void CBuffer::Load(const void * pData, const GLsizeiptr size, const bool recreate) {
  glBindBuffer(mTarget, mId);
  if(recreate) {
    glBufferData(mTarget, size, pData, mUsage);
  }
  else {
    glBufferSubData(mTarget, 0, size, pData);
  }
  glBindBuffer(mTarget, 0);
}
