//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#include "YTE/Core/Engine.hpp"
#include "YTE/Platform/Window.hpp"


namespace YTE
{
  Engine::Engine() : mGraphicsSystem(this), mAudioSystem(this)
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
    ++mCurrentFrame;

    static const std::string logicUpdate = "LogicUpdate";
    LogicUpdate update;
    update.mDt = aDt;

    SendEvent(logicUpdate, &update);

    for (auto &window : mWindows)
    {
      window->SetFrameRate(aDt);
      window->Update();
    }
  }
}