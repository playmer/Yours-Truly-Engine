//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#include "YTE/Core/Engine.hpp"
#include "YTE/Platform/Window.hpp"


namespace YTE
{
  Engine::Engine() : mGraphicsSystem(this)
  {
    mShouldUpdate = true;

    auto window = std::make_unique<Window>(this, "Window", nullptr, nullptr, 1280, 720, nullptr);
    mPrimaryWindow = window.get();
    mWindows.emplace_back(std::move(window));

    mGraphicsSystem.SetUpWindow(mPrimaryWindow);
    mGraphicsSystem.Initialize();
  }

  Engine::~Engine()
  {
  }

  void Engine::Update(float aDt)
  {
    mGraphicsSystem.Update(aDt);

    for (auto &window : mWindows)
    {
      window->SetFrameRate(aDt);
      window->Update();
    }
  }
}