//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#include <stdio.h>
#include <assert.h>
#include <malloc.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN

#include "YTE/Platform/Keyboard.hpp"
#include "YTE/Core/Types.hpp"

namespace YTE
{
  void Keyboard::Update()
  {
    for (i16 i = 0; i < 256; ++i)
    {
      i32 down = GetKeyState(i) & 0x8000;

      Key *key = &mKeys[i];

      key->mPreviousState = key->mCurrentState;

      if (key->mPreviousState == KeyState::Up && down)
      {
        key->mCurrentState = KeyState::Pressed;
      }
      else if (key->mPreviousState == KeyState::Pressed && down)
      {
        key->mCurrentState = KeyState::Down;
      }
      else if ((key->mPreviousState == KeyState::Pressed || key->mPreviousState == KeyState::Down)
        && !down)
      {
        key->mCurrentState = KeyState::Released;
      }
      else if ((key->mPreviousState == KeyState::Released)
        && !down)
      {
        key->mCurrentState = KeyState::Up;
      }
    }
  }
  bool Keyboard::IsKeyPressed(KeyCode aKey)
  {
    auto keyState = mKeys[(int)aKey].mCurrentState;
    return KeyState::Down == keyState || KeyState::Pressed == keyState;
  }

  bool Keyboard::IsKeyOnlyPressedSpecifically(KeyCode aKey)
  {
    auto keyState = mKeys[(int)aKey].mCurrentState;
    return KeyState::Pressed == keyState;
  }
}