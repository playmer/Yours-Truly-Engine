//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#define DIRECTINPUT_VERSION 0x0800
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
#include <Winuser.h>

#include "YTE/Core/Engine.hpp"

#include "YTE/Platform/Windows/WindowData_Windows.hpp"
#include "YTE/Platform/Window.hpp"

namespace YTE
{
  static Window* GetWindowPointer(HWND aWindowHandle)
  {
    return (Window*)(GetWindowLongPtr(aWindowHandle, GWLP_USERDATA));
  }

  static void SetWindowPointer(HWND aWindowHandle, Window *aWindow)
  {
    SetWindowLongPtr(aWindowHandle, GWLP_USERDATA, (LONG_PTR)(aWindow));
  }

  LRESULT CALLBACK WindowsMessagePump(HWND aWindowHandle, UINT aMessage, WPARAM aWParam, LPARAM aLParam)
  {
    Window *window = GetWindowPointer(aWindowHandle);
    WindowData *windowData = window->mPlatformSpecificData.Get<WindowData>();

    switch (aMessage)
    {
      // Window Creation
      case WM_CREATE:
      {
        // This aLParam is our Window pointer
        Window* windowToSet = (Window*)((CREATESTRUCT*)(aLParam))->lpCreateParams;
        WindowData *windowToSetData = windowToSet->mPlatformSpecificData.Get<WindowData>();

        // Set it on the user data section of the window
        SetWindowPointer(aWindowHandle, windowToSet);

        windowToSetData->mWindowHandle = aWindowHandle;
        break;
      }

      // Should probably make sure DefWindowProc doesn't get called here. Return?
      case WM_CLOSE:
      case WM_DESTROY:
      {
        // Only close if we're the most parented window.
        // TODO: Handle the case of having multiple "topmost" windows.
        if (window->mParentWindow == nullptr)
        {
          window->mEngine->mShouldUpdate = false;
        }
        break;
      }
      default:
      {
        break;
      }
    }

    // Probably want to only do this sometimes not every time we pump.
    return DefWindowProc(aWindowHandle, aMessage, aWParam, aLParam);
  }

  Window::Window(Engine *aEngine, const char *aWindowName,
                 const char *aWindowIcon, const char *aCursorIcon, int aWidth,
                 int aHeight, Window *aParentWindow)
    : mEngine(aEngine), mParentWindow(aParentWindow), mHeight(aHeight), mWidth(aWidth)
  {
    WindowData *windowData = mPlatformSpecificData.ConstructAndGet<WindowData>();

    WNDCLASS windowsData;

    if (aWindowName == nullptr)
    {
      aWindowName = "Yours-Truly-Engine";
    }

    if (nullptr == aWindowIcon)
    {
      aWindowIcon = IDI_APPLICATION;
    }

    if (nullptr == aCursorIcon)
    {
      aCursorIcon = IDC_ARROW;
    }

    HWND parentHandle = nullptr;
    if (nullptr != aParentWindow)
    {
      parentHandle = aParentWindow->mPlatformSpecificData.Get<WindowData>()->mWindowHandle;
    }

    windowsData.style = CS_HREDRAW | CS_VREDRAW;

    //TODO: Investigate the following flags
    //mData.mWindowsData.style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    // Set our message pump callback.
    windowsData.lpfnWndProc = WindowsMessagePump;
    windowsData.cbClsExtra = 0;
    windowsData.cbWndExtra = 0;

    // This needs to be changed for multiple windows to work, I think.
    windowsData.hInstance = GetModuleHandleA(nullptr);

    windowsData.hIcon = LoadIconA(nullptr, aWindowIcon);

    windowsData.hCursor = LoadCursorA(nullptr, aCursorIcon);

    windowsData.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    windowsData.lpszMenuName = aWindowName;
    windowsData.lpszClassName = "Yours-Truly-Engine_Game";

    RegisterClass(&windowsData);

    windowData->mWindowHandle = CreateWindowExA(
                                                0,
                                                windowsData.lpszClassName,
                                                windowsData.lpszMenuName,
                                                WS_OVERLAPPEDWINDOW,
                                                CW_USEDEFAULT,
                                                CW_USEDEFAULT,
                                                aWidth,
                                                aHeight,
                                                parentHandle,
                                                nullptr,
                                                windowsData.hInstance,
                                                this
                                               );

    // The window was sized to the values we want for the client area.
    // Add the difference between the two to grow the client area correctly,
    // unless the windows border size changes based on window size or 
    // something but thats weird so I'm not going to deal with that.
    RECT clientSize;
    GetClientRect(windowData->mWindowHandle, &clientSize);

    // Keep the previous position (more or less)
    RECT forPosition;
    GetWindowRect(windowData->mWindowHandle, &forPosition);
    int differenceX = aWidth - clientSize.right;
    int differenceY = aHeight - clientSize.bottom;

    // Try to keep the window centered by assuming the border is spaced
    // evenly (almost certainly wrong on the Y axis if it has a window title)
    MoveWindow(
               windowData->mWindowHandle, 
               forPosition.left - differenceX / 2,
               forPosition.top - differenceY / 2, 
               aWidth + differenceX,
               aHeight + differenceY, 
               false
              );

    ShowWindow(windowData->mWindowHandle, SW_SHOWNORMAL);
    UpdateWindow(windowData->mWindowHandle);
  }

  void Window::CreateOpenGLContext()
  {
    PIXELFORMATDESCRIPTOR pfd =
    {
      sizeof(PIXELFORMATDESCRIPTOR),
      1,
      PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
      PFD_TYPE_RGBA,            //The kind of framebuffer. RGBA or palette.
      32,                        //Colordepth of the framebuffer.
      0, 0, 0, 0, 0, 0,
      0,
      0,
      0,
      0, 0, 0, 0,
      24,                        //Number of bits for the depthbuffer
      8,                        //Number of bits for the stencilbuffer
      0,                        //Number of Aux buffers in the framebuffer.
      PFD_MAIN_PLANE,
      0,
      0, 0, 0
    };

    WindowData *windowData = mPlatformSpecificData.Get<WindowData>();
    windowData->mDeviceContext = GetDC(windowData->mWindowHandle);

    int  letWindowsChooseThisPixelFormat;
    letWindowsChooseThisPixelFormat = ChoosePixelFormat(windowData->mDeviceContext, &pfd);
    SetPixelFormat(windowData->mDeviceContext, letWindowsChooseThisPixelFormat, &pfd);

    windowData->mOpenGLContext = wglCreateContext(windowData->mDeviceContext);
    windowData->mCreatedContext = true;
  }

  void Window::MakeOpenGLContextCurrent()
  {
    WindowData *windowData = mPlatformSpecificData.Get<WindowData>();
    wglMakeCurrent(windowData->mDeviceContext, windowData->mOpenGLContext);
  }

  void Window::DestroyOpenGLContext()
  {
    WindowData *windowData = mPlatformSpecificData.Get<WindowData>();

    if (windowData->mCreatedContext)
    {
      wglDeleteContext(windowData->mOpenGLContext);
    }
  }

  void Window::SwapBuffers()
  {
    WindowData *windowData = mPlatformSpecificData.Get<WindowData>();
    ::SwapBuffers(windowData->mDeviceContext);
  }


  void Window::Update()
  {
    WindowData *windowData = mPlatformSpecificData.Get<WindowData>();

    MSG message;
    while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
    {

      TranslateMessage(&message);
      DispatchMessage(&message);
    }

    RedrawWindow(windowData->mWindowHandle, NULL, NULL, RDW_INTERNALPAINT);

    mKeyboard.Update();
  }
}
