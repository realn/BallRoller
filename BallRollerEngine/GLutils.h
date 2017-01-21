#pragma once

#ifdef TARGET_IOS
#include <OpenGLES/ES2/gl.h>
#else 
#include <GLES2/gl2.h>
#endif

#include <glm/glm.hpp>

template<typename _Type>
static const GLenum GetGLType() {
  return 0;
}

template<>
static const GLenum GetGLType<glm::int8>() {
  return GL_BYTE;
}

template<>
static const GLenum GetGLType<glm::uint8>() {
  return GL_UNSIGNED_BYTE;
}

template<>
static const GLenum GetGLType<glm::int16>() {
  return GL_SHORT;
}

template<>
static const GLenum GetGLType<glm::uint16>() {
  return GL_UNSIGNED_SHORT;
}

template<>
static const GLenum GetGLType<glm::int32>() {
  return GL_INT;
}

template<>
static const GLenum GetGLType<glm::uint32>() {
  return GL_UNSIGNED_INT;
}

template<>
static const GLenum GetGLType<glm::float32>() {
  return GL_FLOAT;
}

template<class _BindClass> 
class CBind {
private:
  const _BindClass* mObj;
public:
  CBind(const _BindClass* obj) : mObj(obj) {
    mObj->Bind();
  }
  ~CBind() {
    mObj->Unbind();
  }
};

class CGLState {
private:
  GLenum mState;
  GLboolean mPrevVal;
public:
  CGLState(const GLenum state, const bool enable = true) : mState(state) {
    mPrevVal = glIsEnabled(mState);
    Set(enable);
  }
  ~CGLState() {
    Set(mPrevVal == GL_TRUE);
  }

  void Set(const bool enable) const {
    if(enable)
      glEnable(mState);
    else
      glDisable(mState);
  }
};
