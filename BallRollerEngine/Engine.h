#pragma once

#include "GLutils.h"
#include "Device.h"
#include <vector>

class CMesh;
class CTexture;
class CShaderProgram;
class CFont;
class CFontText;

class CEngine : public IEngine {
private:
  IDevice* mDevice;

  glm::mat4 mProj;
  glm::mat4 mView;

  GLint mUniTransform;
  GLint mUniTexBase;

  float mRotation;
  float mRotVel;

  CMesh* mpMesh;
  CTexture* mTexture;
  CShaderProgram* mProgram;
  CFont* mFont;
  CFontText* mText;

public:
  CEngine(IDevice* pDevice);
  ~CEngine();

  const bool Initialize() override;
  void Release() override;

  void ScreenChanged(const glm::int32 width, const glm::int32 height) override;
  void FrameUpdate(const glm::float32 timeDelta) override;

  void UserAction(const UserActionType type) override;

private:
  void  Update(const glm::float32 timeDelta);
  void  Render();
};