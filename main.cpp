//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#include <stdio.h>
#include <iostream>

#include "YTE/Core/Engine.hpp"
#include "YTE/Platform/Windows/WindowData_Windows.hpp"

#include <memory>

#include "YTE/DataStructures/UniqueHandle.hpp"

struct Color
{
  Color() : mRed(0.0f), mGreen(0.0f), mBlue(0.0f), mAlpha(1.0f) {};

  float mRed;
  float mGreen;
  float mBlue;
  float mAlpha;
};


int main(int aArgumentNumber, char **Arguments)
{
  YTE::Engine engine;

  // This is how to create SubWindows
  //YTE::Window subWindow1( &engine, "subWindow1", nullptr, nullptr, 246, 360, &engine.mWindow );


  while (engine.mShouldUpdate)
  {
    engine.Update(0.016f);

    if (engine.mWindow.mKeyboard.IsKeyPressed(YTE::KeyCode::Escape))
    { 
      engine.mShouldUpdate = false;
    }
  }

  return 0;
}