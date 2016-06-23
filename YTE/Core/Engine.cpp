//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#include "YTE/Core/Engine.hpp"
#include "YTE/Platform/Window.hpp"


namespace YTE
{
  Engine::Engine() : mWindow(this, "Window", nullptr, nullptr, 1280, 720, nullptr)
  {
    mShouldUpdate = true;
    mWindows
  }

  Engine::~Engine()
  {
  }

  void Engine::Update(float aDt)
  {
    mWindow.Update();
  }
}