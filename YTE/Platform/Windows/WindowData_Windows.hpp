/******************************************************************************/
// author Joshua T. Fisher
// date   11/26/2015
/******************************************************************************/
#pragma once

#ifndef WindowData_Windows_hpp

#define WindowData_Windows_hpp
#define DIRECTINPUT_VERSION 0x0800
#define WIN32_LEAN_AND_MEAN 1
#define VC_EXTRALEAN
#include <Windows.h>
#include <Winuser.h>


namespace YTE
{
  struct WindowData
  {
    WindowData()
    {
      ZeroMemory(this, sizeof(WindowData));
    }

    HWND mWindowHandle;
    HDC mDeviceContext;
    HGLRC mOpenGLContext;
    HINSTANCE mInstance;
    bool mCreatedContext;
  };
}

#endif
