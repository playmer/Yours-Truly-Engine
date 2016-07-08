//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#include <stdio.h>
#include <iostream>

#include "YTE/Core/Engine.hpp"
#include "YTE/Platform/Windows/WindowData_Windows.hpp"

#include <memory>

#include "YTE/DataStructures/UniqueHandle.hpp"
#include "glm.hpp"
#include <gtc/matrix_transform.hpp>


int main(int aArgumentNumber, char **Arguments)
{
  YTE::Engine engine;

  // This is how to create SubWindows
  //YTE::Window subWindow1( &engine, "subWindow1", nullptr, nullptr, 246, 360, &engine.mWindow );


  while (engine.mShouldUpdate)
  {
    float dt = 0.016f;
    engine.Update(dt);

    if (engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::Escape))
    { 
      engine.mShouldUpdate = false;
    }

    glm::vec4 transform;

    if (engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::Right) ||
        engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::D))
    {
      transform.x += 0.5f * dt;
    }
    if (engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::Left) ||
        engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::A))
    {
      transform.x += -0.5f * dt;
    }
    if (engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::Up) ||
        engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::W))
    {
      transform.y += -0.5f * dt;
    }
    if (engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::Down) ||
        engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::S))
    {
      transform.y += 0.5f * dt;
    }

    for (YTE::u8 i = 0; i < 3; ++i)
    {
      engine.mGraphicsSystem.mTriangle[i] += transform;
    }
  }

  return 0;
}