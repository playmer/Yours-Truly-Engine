//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#include "YTE/Core/Engine.hpp"

#include "YTE/Platform/Window.hpp"

namespace YTE
{
  Window::~Window()
  {
    //DestroyOpenGLContext();
  }


  void Window::SetFrameRate(float aDt)
  {
    static double totalTime = 0.0;
    totalTime += aDt;

    static u16 counter = 0;
    ++counter;

    if (totalTime >= 0.5)
    {
      std::string windowText =  "FPS: " + std::to_string((counter / 0.5));

      SetWindowTitle(windowText);

      totalTime = 0.0f;
      counter = 0;
    }
  }
}
