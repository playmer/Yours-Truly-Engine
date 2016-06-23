//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef Engine_hpp
#define Engine_hpp

#include <stddef.h>
#include <memory>
#include <vector>

#include "YTE/Platform/Window.hpp"

namespace YTE
{
  class Engine
  {
  public:
    Engine();
    ~Engine();
    void Update(float aDt);

    std::vector<std::unique_ptr<Window>> mWindows;

    bool mShouldUpdate;
  };
}
#endif
