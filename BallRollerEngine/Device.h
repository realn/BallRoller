#pragma once

#include <string>

class IDevice {
public:
  virtual void Log(const std::string& text) = 0;
};