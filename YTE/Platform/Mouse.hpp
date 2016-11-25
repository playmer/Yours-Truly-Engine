//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef Mouse_hpp
#define Mouse_hpp

#include "YTE/Core/Types.hpp"

namespace YTE
{
  class Mouse
  {
    public:
    Mouse() : mWheelDelta(0.0f) {};

    float mWheelDelta;
    u32 mX = 0;
    u32 mY = 0;

    bool mLeftMouseDown = false;
  };
}
#endif