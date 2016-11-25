//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////

#pragma once

#ifndef Keyboard_hpp
#define Keyboard_hpp

#include "YTE/Core/Types.hpp"

#include "YTE/Platform/KeyCodes.hpp"

namespace YTE
{
  enum class KeyState : byte
  {
    Pressed = 0,
    Down = 1,
    Released = 2,
    Up = 3,
  };

  struct Key
  {
    KeyState mPreviousState : 2;
    KeyState mCurrentState : 2;
  };

  class Keyboard
  {
  public:
    Keyboard();
    ~Keyboard();
    void Update();
    
    bool IsKeyPressed(KeyCode aKey);
    bool IsKeyOnlyPressedSpecifically(KeyCode aKey);

    Key mKeys[256];
  };
}
#endif
