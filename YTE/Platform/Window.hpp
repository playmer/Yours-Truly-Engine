//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef Window_h
#define Window_h

#include <stddef.h>

#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/PrivateImplementation.hpp"

#include "YTE/Platform/Keyboard.hpp"
#include "YTE/Platform/Mouse.hpp"

namespace YTE
{
  class Window
  {
  public:
    Window(Engine *aEngine, const char *aWindowName,
           const char *aWindowIcon, const char *aCursorIcon, int aWidth,
           int aHeight, Window *aParentWindow);

    ~Window();

    void Update();

    void CreateOpenGLContext();
    void MakeOpenGLContextCurrent();
    void DestroyOpenGLContext();
    void SwapBuffers();

    void SetFrameRate(float aDt);
    void SetWindowTitle(const std::string &aWindowText);

    Keyboard mKeyboard;
    Mouse mMouse;
    PrivateImplementationLocal<64> mPlatformSpecificData;
    Engine *mEngine;
    Window *mParentWindow;
    u16 mHeight;
    u16 mWidth;
  };
}
#endif