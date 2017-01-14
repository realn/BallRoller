#pragma once

#include <string>
#include <vector>

class IDevice {
public:
    virtual void Log(const std::string& text) = 0;
    virtual bool LoadAsset(const std::string& name, std::vector<unsigned char>& outData) = 0;
};