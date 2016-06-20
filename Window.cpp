//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#include "YTE/Core/Engine.hpp"

#include "YTE/Platform/Window.hpp"

namespace YTE
{
  Window::~Window()
  {
    DestroyOpenGLContext();
  }
}
