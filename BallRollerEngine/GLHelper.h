#pragma once

#include <GLES2/gl2.h>
#include <glm/glm.hpp>


template<typename _Type>
static const GLenum GetGLType() {
  return 0;
}

template<>
static const GLenum GetGLType<char>() {
  return GL_BYTE;
}

template<>
static const GLenum GetGLType<unsigned char>() {
  return GL_UNSIGNED_BYTE;
}

template<>
static const GLenum GetGLType<short>() {
  return GL_SHORT;
}

template<>
static const GLenum GetGLType<unsigned short>() {
  return GL_UNSIGNED_SHORT;
}

template<>
static const GLenum GetGLType<int>() {
  return GL_INT;
}

template<>
static const GLenum GetGLType<unsigned int>() {
  return GL_UNSIGNED_INT;
}

template<>
static const GLenum GetGLType<float>() {
  return GL_FLOAT;
}
