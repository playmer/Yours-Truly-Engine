//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#include <stdio.h>
#include <assert.h>
#include <malloc.h>

#include "YTE/Platform/Keyboard.hpp"
//#include "YTE/Platform/KeyCodes.h"
#include "YTE/Core/Types.hpp"

namespace YTE
{
  Keyboard::Keyboard()
  {
    for (i16 i = 0; i < 256; ++i)
    {
      Key *key = &mKeys[i];

      key->mPreviousState = key->mCurrentState = KeyState::Up;
    }
  }

  Keyboard::~Keyboard()
  {
  }
}