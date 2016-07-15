//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#include <stdio.h>
#include <iostream>

#include "YTE/Core/Engine.hpp"
//#include "YTE/Platform/Windows/WindowData_Windows.hpp"

#include <memory>

#include <chrono>

#include "YTE/DataStructures/UniqueHandle.hpp"
#include "glm.hpp"
#include <gtc/matrix_transform.hpp>


int main(int aArgumentNumber, char **Arguments)
{
  YTE::Engine engine;

  // This is how to create SubWindows
  //YTE::Window subWindow1( &engine, "subWindow1", nullptr, nullptr, 246, 360, &engine.mWindow );


  std::chrono::time_point<std::chrono::high_resolution_clock> begin = std::chrono::high_resolution_clock::now();
  std::chrono::time_point<std::chrono::high_resolution_clock> lastFrame = begin;

  while (engine.mShouldUpdate)
  {
    std::chrono::duration<float> timeSpan = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::high_resolution_clock::now() - lastFrame);
    lastFrame = std::chrono::high_resolution_clock::now();
    auto dt = timeSpan.count();;

    engine.Update(dt);

    if (engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::Escape))
    { 
      engine.mShouldUpdate = false;
    }

    glm::vec3 transform = glm::vec3();
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 rotate = glm::vec3();

    // Translation
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

    engine.mGraphicsSystem.mQuad->Translate(transform);



    // Scaling
    if (engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::N1))
    {
      scale.x += 0.5f * dt;
    }
    if (engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::N2))
    {
      scale.x += -0.5f * dt;
    }
    if (engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::N3))
    {
      scale.y += -0.5f * dt;
    }
    if (engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::N4))
    {
      scale.y += 0.5f * dt;
    }

    engine.mGraphicsSystem.mQuad->Scale(scale);

    // Rotation
    //About X Axis
    if (engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::N0))
    {
      rotate.x += glm::pi<float>()/2 * dt;
    }
    if (engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::N9))
    {
      rotate.x -= glm::pi<float>()/2 * dt;
    }

    //About Y Axis
    if (engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::N8))
    {
      rotate.y += glm::pi<float>() / 2 * dt;
    }
    if (engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::N7))
    {
      rotate.y -= glm::pi<float>() / 2 * dt;
    }

    //About Z Axis
    if (engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::N6))
    {
      rotate.z += glm::pi<float>() / 2 * dt;
    }
    if (engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::N5))
    {
      rotate.z -= glm::pi<float>() / 2 * dt;
    }

    engine.mGraphicsSystem.mQuad->Rotate(rotate);
  }

  return 0;
}