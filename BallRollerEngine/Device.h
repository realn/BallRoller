#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

enum class UserActionType {
  Tap = 0,
  DbTap = 1,
  Cancel = 2,
  MoveLeft = 3,
  MoveRight = 4,
  MoveUp = 5,
  MoveDown = 6,
};

class IDevice {
public:
    virtual void Log(const std::string& text) = 0;
    virtual bool LoadAsset(const std::string& name, std::vector<glm::uint8>& outData) = 0;
};

class IEngine {
protected:
  IEngine() {}
  IEngine(const IEngine&) {}
public:
  ~IEngine() {}

  virtual const bool Initialize() = 0;
  virtual void Release() = 0;
  virtual void ScreenChanged(const glm::int32 width, const glm::int32 height) = 0;
  virtual void FrameUpdate(const glm::float32 timeDelta) = 0;
  virtual void UserAction(const UserActionType type) = 0;

  static IEngine* Create(IDevice* pDevice);
};