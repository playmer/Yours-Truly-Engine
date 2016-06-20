//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef Engine_hpp
#define Engine_hpp

#include <stddef.h>

#include "YTE/Platform/Window.hpp"

namespace YTE
{
  class Engine
  {
  public:
    Engine();
    ~Engine();
    void Update(float aDt);

    Window mWindow;

    bool mShouldUpdate;
  };
}
#endif
