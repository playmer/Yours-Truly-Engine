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


YTE::Object MakeObject(YTE::GraphicsSystem *aGraphicsSystem, const char *aTextureFile)
{
  auto context = aGraphicsSystem->mPlatformSpecificData.Get<YTE::VulkanContext>();
  YTE::Object object;

  YTE::TextureLoader loader(context->mPhysicalDevice, context->mLogicalDevice, context->mQueue, context->mCommandPool);
  object.mTexture = loader.loadTexture(aTextureFile); // TODO: Format is wrong.
  
  object.mIndicies = context->CreateIndexBuffer({ 0, 1, 2, 2, 3, 0 }, false);

  YTE::Vertex mVertex1;

  mVertex1.mPosition = { -0.5f, -0.5f, 0.0f, 1.0f };
  mVertex1.mUVCoordinates = { 0.0f, 0.0f };
  mVertex1.mNormal = { 0.0f, 0.0f, 1.0 };

  YTE::Vertex mVertex2;

  mVertex2.mPosition = { 0.5f, -0.5f, 0.0f, 1.0f };
  mVertex2.mUVCoordinates = { 1.0f, 0.0f };
  mVertex2.mNormal = { 0.0f, 0.0f, 1.0 };
  
  YTE::Vertex mVertex3;

  mVertex3.mPosition = { 0.5f, 0.5f, 0.0f, 1.0f };
  mVertex3.mUVCoordinates = { 1.0f, 1.0f };
  mVertex3.mNormal = { 0.0f, 0.0f, 1.0 };

  YTE::Vertex mVertex4;

  mVertex4.mPosition = { -0.5f, 0.5f, 0.0f, 1.0f };
  mVertex4.mUVCoordinates = { 0.0f, 1.0f };
  mVertex4.mNormal = { 0.0f, 0.0f, 1.0 };

  object.mVerts = context->CreateVertexBuffer({ mVertex1, mVertex2, mVertex3, mVertex4 }, false);

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

  engine.mGraphicsSystem.mObjects.push_back(MakeObject(&engine.mGraphicsSystem, "./Textures/Skeleman.png"));
  engine.mGraphicsSystem.mObjects[0].mTranslation = { 1.0, 1.0, 0.0 };

  engine.mGraphicsSystem.mObjects.push_back(MakeObject(&engine.mGraphicsSystem, "./Textures/Happy.png"));
  

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

    engine.mGraphicsSystem.mObjects[1].mTranslation = translation;
    engine.mGraphicsSystem.mObjects[1].mScale = scale;
    engine.mGraphicsSystem.mObjects[1].mRotate = rotate;
  }

  return 0;
}