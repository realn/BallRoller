#pragma once

#include <string>
#include <vector>

class IDevice {
public:
    virtual void Log(const std::string& text) = 0;
    virtual bool LoadPng(const std::string& name, std::vector<int>& outData, int& outWidth, int& outHeight) = 0;
};