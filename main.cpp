//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#include <stdio.h>
#include <iostream>

#include <chrono>
#include <memory>
#include <random>

#include "YTE/Core/Engine.hpp"
//#include "YTE/Platform/Windows/WindowData_Windows.hpp"


#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/Instance.hpp"
#include "YTE/Graphics/TextureLoader.hpp"
#include "YTE/Graphics/VulkanContext.hpp"

#include "glm.hpp"
#include <gtc/matrix_transform.hpp>

#include "YTE/StandardLibrary/CompileTimeString.hpp"


#include "YTE/Audio/AudioSystem.hpp"


YTE::Instance MakeObject(YTE::GraphicsSystem *aGraphicsSystem, glm::vec3 aColor = glm::vec3(-1, -1, -1))
{
  auto context = aGraphicsSystem->mPlatformSpecificData.Get<YTE::VulkanContext>();
  YTE::Instance object(aGraphicsSystem);
  object.SetColor(aColor);
  return object;
}

glm::vec3 randomVector(float a, float b, std::default_random_engine &e1)
  
{
  std::uniform_real_distribution<float> dist{ a, b };

  return glm::vec3(dist(e1), dist(e1), dist(e1));
}

int main(int aArgumentNumber, char **Arguments)
{
  //constexpr YTE::CompileTimeString test1{ "Test" };
  //constexpr YTE::CompileTimeString test2{ "Test" };
  //constexpr bool areTheySame = (test1 == test2);

  YTE::Engine engine;

  // This is how to create SubWindows
  //YTE::Window subWindow1( &engine, "subWindow1", nullptr, nullptr, 246, 360, &engine.mWindow );


  std::chrono::time_point<std::chrono::high_resolution_clock> begin = std::chrono::high_resolution_clock::now();
  std::chrono::time_point<std::chrono::high_resolution_clock> lastFrame = begin;

  glm::vec3 translation = glm::vec3();
  glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
  glm::vec3 rotate = glm::vec3();

  //engine.mGraphicsSystem.mInstances.push_back(MakeObject(&engine.mGraphicsSystem, glm::vec3(0, 1, 0)));
  //engine.mGraphicsSystem.mInstances[0].SetTranslation({ 1.0, 1.0, 0.25 });
  //engine.mGraphicsSystem.mInstances[0].SetScale({ 1.0, 2.0, 0.0 });
  //engine.mGraphicsSystem.mInstances[0].SetRotation({ 0.0, 0.0, glm::pi<float>() / 4.0f });
  //engine.mGraphicsSystem.mInstances.push_back(MakeObject(&engine.mGraphicsSystem, glm::vec3(1, 0, 0)));
  //engine.mGraphicsSystem.mInstances[1].SetTranslation({ 1.0, -1.0, 0.0 });
  //engine.mGraphicsSystem.mInstances[1].SetScale({ 2.0, 1.0, 0.0 });
  //engine.mGraphicsSystem.mInstances[1].SetRotation({ 0.0, 0.0, glm::pi<float>() / 4.0f });
  //engine.mGraphicsSystem.mInstances.push_back(MakeObject(&engine.mGraphicsSystem, glm::vec3(0, 0, 1)));
  //engine.mGraphicsSystem.mInstances[2].SetTranslation({ -1.0, -1.0, 0.0 });
  //engine.mGraphicsSystem.mInstances[2].SetScale({ 0.5, 1.0, 0.0 });
  //engine.mGraphicsSystem.mInstances[2].SetRotation({ 0.0, 0.0, glm::pi<float>() / 4.0f });
  //engine.mGraphicsSystem.mInstances.push_back(MakeObject(&engine.mGraphicsSystem));
  //engine.mGraphicsSystem.mInstances[3].SetTranslation({ -1.0, 1.0, 0.0 });
  //engine.mGraphicsSystem.mInstances[3].SetScale({ 1.0, 0.5, 0.0 });
  //engine.mGraphicsSystem.mInstances[3].SetRotation({ 0.0, 0.0, glm::pi<float>() / 4.0f });
  //engine.mGraphicsSystem.mInstances[3].SetTextureId(1);
  //
  //engine.mGraphicsSystem.mInstances.push_back(MakeObject(&engine.mGraphicsSystem));

  std::random_device r;
  std::default_random_engine e1{ r() };

  for (size_t i = 5; i < 2000; ++i)
  {
    //engine.mGraphicsSystem.mInstances.push_back(MakeObject(&engine.mGraphicsSystem, randomVector(0.0f, 1.0f, e1)));
    //engine.mGraphicsSystem.mInstances[i].SetTranslation(randomVector(-20.0f, 20.0f, e1));
    //engine.mGraphicsSystem.mInstances[i].SetScale(randomVector(0.2f, 3.0f, e1));
    //engine.mGraphicsSystem.mInstances[i].SetRotation(randomVector(0.0f, glm::pi<float>(), e1));
    //engine.mGraphicsSystem.mInstances[i].SetTextureId(1);
  }

  auto context = engine.mGraphicsSystem.mPlatformSpecificData.Get<YTE::VulkanContext>();


  //auto panelMusicIGuessWeCanCallItThat = engine.mAudioSystem.PlayLoop("Panel", 1.0);

  while (engine.mShouldUpdate)
  {
    std::chrono::duration<float> timeSpan = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::high_resolution_clock::now() - lastFrame);
    lastFrame = std::chrono::high_resolution_clock::now();
    auto dt = timeSpan.count();;

    engine.Update(dt);


    const float zoomSpeed = 0.15f;
    const float rotationSpeed = 1.25f;

    // Update rotation
    auto &mouse = engine.mPrimaryWindow->mMouse;
    if (true == mouse.mLeftMouseDown)
    {
      context->mRotation.x += (engine.mGraphicsSystem.mMousePosition.y - (float)mouse.mY) * rotationSpeed;
      context->mRotation.y -= (engine.mGraphicsSystem.mMousePosition.x - (float)mouse.mX) * rotationSpeed;

      engine.mGraphicsSystem.mMousePosition.x = mouse.mX;
      engine.mGraphicsSystem.mMousePosition.y = mouse.mY;
    }

    // Update zoom
    auto zoom = context->mZoom;
    context->mZoom += engine.mPrimaryWindow->mMouse.mWheelDelta * zoomSpeed;

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
    if (engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::Z))
    {
      translation.z += -0.5f * dt;
    }
    if (engine.mPrimaryWindow->mKeyboard.IsKeyPressed(YTE::KeyCode::X))
    {
      translation.z += 0.5f * dt;
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

    //engine.mGraphicsSystem.mInstances[4].SetTranslation(translation);
    //engine.mGraphicsSystem.mInstances[4].SetScale(scale);
    //engine.mGraphicsSystem.mInstances[4].SetRotation(rotate);

    if (engine.mPrimaryWindow->mKeyboard.IsKeyOnlyPressedSpecifically(YTE::KeyCode::M))
    {
      //engine.mAudioSystem.PlayOnce("Alarm");
    }
  }

  return 0;
}