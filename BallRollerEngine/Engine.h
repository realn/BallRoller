#pragma once

#include <glm/glm.hpp>

#include <GLES2/gl2.h>

class IDevice;

class CEngine {
private:
  IDevice* mDevice;

  glm::mat4 mProj;
  glm::mat4 mView;

  GLint mAttrVert;
  GLint mAttrColor;
  GLint mUniTransform;

  GLint mShaderProgram;

public:
  CEngine(IDevice* pDevice);
  ~CEngine();

  bool Initialize();
  void Release();

  void ScreenChanged(int width, int height);

  void FrameUpdate(const float timeDelta);

private:
  void  Update(const float timeDelta);
  void  Render();

  GLint CreateShader(const GLenum type, const std::string& source);
  GLint CreateShaderProgram(const std::vector<GLint>& shaders);
};