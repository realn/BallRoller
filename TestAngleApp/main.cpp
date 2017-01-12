#include "stdafx.h"
#include <vector>
#include <iostream>

#define SDL_MAIN_HANDLED

#define GL_GLEXT_PROTOTYPES

#include <EGL/egl.h>
#include <SDL.h>
#include <SDL_syswm.h>

#include <GLES2/gl2.h>

#include "../BallRollerEngine/Engine.h"
#include "../BallRollerEngine/Device.h"

void addAttrib(std::vector<EGLint>& atts, EGLint attrib, EGLint value) {
  atts.push_back(attrib);
  atts.push_back(value);
}

class CWinDevice : public IDevice {
public:
  // Inherited via IDevice
  virtual void Log(const std::string & text) override {
    std::cout << text << std::endl;
  }

  // Inherited via IDevice
  virtual bool LoadPng(const std::string & name, std::vector<int>& outData, int & outWidth, int & outHeight) override {
    
    return false;
  }

};

void main() {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

  SDL_Window* pWindow = SDL_CreateWindow("TestApp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320, 480, 0);
  SDL_SysWMinfo info;

  SDL_ShowWindow(pWindow);

  SDL_VERSION(&info.version);
  SDL_GetWindowWMInfo(pWindow, &info);

  EGLDisplay display;
  #if defined(WINDOWS) || defined(WIN32)
  HWND handle = info.info.win.window;
  HDC hdc = GetDC(handle);
  display = eglGetDisplay(hdc);
  #else
  display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  #endif

  if(!eglInitialize(display, NULL, NULL)) {
    return;
  }

  std::vector<EGLint> atts;
  addAttrib(atts, EGL_BUFFER_SIZE, 32);
  addAttrib(atts, EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER);
  addAttrib(atts, EGL_CONFORMANT, EGL_OPENGL_ES2_BIT);
  addAttrib(atts, EGL_DEPTH_SIZE, 24);
  atts.push_back(EGL_NONE);

  EGLint configNum;
  EGLConfig config;
  eglChooseConfig(display, &atts[0], &config, 1, &configNum);

  if(configNum <= 0)
    return;


  std::vector<EGLint> winAtts;
  winAtts.push_back(EGL_NONE);
  EGLSurface windowSurface = eglCreateWindowSurface(display, config, handle, &winAtts[0]);

  std::vector<EGLint> ctxAtts;
  addAttrib(ctxAtts, EGL_CONTEXT_CLIENT_VERSION, 2);
  ctxAtts.push_back(EGL_NONE);
  EGLContext context = eglCreateContext(display, config, NULL, &ctxAtts[0]);

  eglMakeCurrent(display, windowSurface, windowSurface, context);

  CWinDevice device;
  CEngine engine(&device);

  engine.Initialize();
  {
    int w, h;
    SDL_GetWindowSize(pWindow, &w, &h);

    engine.ScreenChanged(w, h);
  }

  Uint64 timerFreq = SDL_GetPerformanceFrequency();
  Uint64 timerPrev = SDL_GetPerformanceCounter();

  SDL_Event event;
  bool run = true;
  while(run) {
    SDL_PollEvent(&event);
    if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
      run = false;
    }
    if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
      engine.ScreenChanged(event.window.data1, event.window.data2);
    }

    Uint64 timerNow = SDL_GetPerformanceCounter();
    float deltaTime = (float)(timerNow - timerPrev) / timerFreq;
    timerPrev = timerNow;

    engine.FrameUpdate(deltaTime);

    eglSwapBuffers(display, windowSurface);
  }

  engine.Release();

  eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

  eglDestroyContext(display, context);
  eglDestroySurface(display, windowSurface);

  eglTerminate(display);

  SDL_DestroyWindow(pWindow);

  SDL_Quit();
}