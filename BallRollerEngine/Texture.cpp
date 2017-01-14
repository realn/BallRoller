#include "stdafx.h"
#include "Texture.h"
#include "lodepng.h"

CTexture::CTexture(const GLenum format, const glm::uvec2 size) 
  : mId(0), mTarget(GL_TEXTURE_2D), mFormat(format), mSize(size) {
  glGenTextures(1, &mId);
  glBindTexture(mTarget, mId);
  ApplyParams();
  glTexImage2D(mTarget, 0, mFormat, mSize.x, mSize.y, 0, mFormat, GL_UNSIGNED_BYTE, nullptr);
  glGenerateMipmap(mTarget);
  glBindTexture(mTarget, 0);
}

CTexture::~CTexture() {
  glDeleteTextures(1, &mId);
}

void CTexture::Bind(const GLuint unit) const {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(mTarget, mId);
  glEnable(mTarget);
}

void CTexture::Unbind(const GLuint unit) const {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(mTarget, 0);
  glDisable(mTarget);
}

void CTexture::Load(const GLenum inputFormat, GLenum dataFormat, const void * pData) {
  glBindTexture(mTarget, mId);
  glTexSubImage2D(mTarget, 0, 0, 0, mSize.x, mSize.y, inputFormat, dataFormat, pData);
  glBindTexture(mTarget, 0);
}

void CTexture::Load(const glm::uvec2 size, const GLenum format, const GLenum inputFormat, const GLenum dataFormat, const void * pData) {
  glBindTexture(mTarget, mId);
  mSize = size;
  mFormat = format;

  ApplyParams();
  glTexImage2D(mTarget, 0, mFormat, mSize.x, mSize.y, 0, inputFormat, dataFormat, pData);
  glGenerateMipmap(mTarget);
  GLint err = glGetError();
  glBindTexture(mTarget, 0);
}

const bool CTexture::LoadPng(const std::vector<unsigned char>& pngData) {
  if(pngData.empty())
    return false;

  unsigned int w, h;
  std::vector<unsigned char> data;
  int err = lodepng::decode(data, w, h, pngData);
  if(err != 0) {
    return false;
  }
  
  Load(glm::uvec2(w, h), GL_RGBA, GL_RGBA, data);
  return true;
}

void CTexture::ApplyParams() {
  glTexParameteri(mTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(mTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

