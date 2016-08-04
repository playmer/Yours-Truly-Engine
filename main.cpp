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
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/TextureLoader.hpp"
#include "YTE/Graphics/VulkanContext.hpp"

#include "glm.hpp"
#include <gtc/matrix_transform.hpp>


YTE::Object MakeObject(YTE::GraphicsSystem *aGraphicsSystem, glm::vec3 aColor = glm::vec3(-1, -1, -1))
{
  auto context = aGraphicsSystem->mPlatformSpecificData.Get<YTE::VulkanContext>();
  YTE::Object object;
  object.mColor = aColor;
  return object;
}


int main(int aArgumentNumber, char **Arguments)
{
  YTE::Engine engine;

  // This is how to create SubWindows
  //YTE::Window subWindow1( &engine, "subWindow1", nullptr, nullptr, 246, 360, &engine.mWindow );


  std::chrono::time_point<std::chrono::high_resolution_clock> begin = std::chrono::high_resolution_clock::now();
  std::chrono::time_point<std::chrono::high_resolution_clock> lastFrame = begin;

  glm::vec3 translation = glm::vec3();
  glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
  glm::vec3 rotate = glm::vec3();

  engine.mGraphicsSystem.mObjects.push_back(MakeObject(&engine.mGraphicsSystem, glm::vec3(0, 1, 0)));
  engine.mGraphicsSystem.mObjects[0].mTranslation = { 1.0, 1.0, 0.0 };
  engine.mGraphicsSystem.mObjects.push_back(MakeObject(&engine.mGraphicsSystem, glm::vec3(1, 0, 0)));
  engine.mGraphicsSystem.mObjects[1].mTranslation = { 1.0, -1.0, 0.0 };
  engine.mGraphicsSystem.mObjects.push_back(MakeObject(&engine.mGraphicsSystem, glm::vec3(0, 0, 1)));
  engine.mGraphicsSystem.mObjects[2].mTranslation = { -1.0, -1.0, 0.0 };
  engine.mGraphicsSystem.mObjects.push_back(MakeObject(&engine.mGraphicsSystem, glm::vec3(0.5, 0.5, 0.5)));
  engine.mGraphicsSystem.mObjects[3].mTranslation = { -1.0, 1.0, 0.0 };

  engine.mGraphicsSystem.mObjects.push_back(MakeObject(&engine.mGraphicsSystem));
  

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

    // Translation
    if (engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::Right) ||
        engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::D))
    {
      translation.x += 0.5f * dt;
    }
    if (engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::Left) ||
        engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::A))
    {
      translation.x += -0.5f * dt;
    }
    if (engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::Up) ||
        engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::W))
    {
      translation.y += -0.5f * dt;
    }
    if (engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::Down) ||
        engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::S))
    {
      translation.y += 0.5f * dt;
    }

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

    engine.mGraphicsSystem.mObjectsBufferPtr[4].mTranslation = translation;
    engine.mGraphicsSystem.mObjectsBufferPtr[4].mScale = scale;
    engine.mGraphicsSystem.mObjectsBufferPtr[4].mRotate = rotate;
  }

  return 0;
}