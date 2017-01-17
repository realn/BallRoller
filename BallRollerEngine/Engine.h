#pragma once

#include <glm/glm.hpp>
#include <vector>

#include <GLES2/gl2.h>

class IDevice;
class CMesh;
class CTexture;

enum class UserActionType {
  Click = 0,
  DbClick = 1,
  Cancel = 2,
  MoveLeft = 3,
  MoveRight = 4,
  MoveUp = 5,
  MoveDown = 6,
};

class CEngine {
private:
  IDevice* mDevice;

  glm::mat4 mProj;
  glm::mat4 mView;

  GLint mAttrPos;
  GLint mAttrCoord;
  GLint mUniTransform;
  GLint mUniTexBase;

  GLint mShaderProgram;

  float mRotation;
  float mRotVel;

  CMesh* mpMesh;
  CTexture* mTexture;

public:
  CEngine(IDevice* pDevice);
  ~CEngine();

  bool Initialize();
  void Release();

  void ScreenChanged(int width, int height);

  void FrameUpdate(const float timeDelta);

  void UserAction(UserActionType type);

private:
  void  Update(const float timeDelta);
  void  Render();

  GLint CreateShader(const GLenum type, const std::string& source);
  GLint CreateShaderProgram(const std::vector<GLint>& shaders);
  GLint CreateShaderProgram(const int numberOfShaders, ...);
  void GLcheck(const std::string& text);
};