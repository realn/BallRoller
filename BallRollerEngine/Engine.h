#pragma once

#include <glm/glm.hpp>
#include <vector>

#include <GLES2/gl2.h>

class IDevice;
class CMesh;

class CEngine {
private:
  IDevice* mDevice;

  glm::mat4 mProj;
  glm::mat4 mView;

  GLint mAttrPos;
  GLint mAttrColor;
  GLint mUniTransform;

  GLint mShaderProgram;

  float mRotation;

  CMesh* mpMesh;

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
  GLint CreateShaderProgram(const int numberOfShaders, ...);
  void GLcheck(const std::string& text);
};