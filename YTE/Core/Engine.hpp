//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef Engine_hpp
#define Engine_hpp

#include <stddef.h>
#include <memory>
#include <vector>

#include "YTE/Audio/AudioSystem.hpp"

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"

#include "YTE/Platform/Window.hpp"

namespace YTE
{

  class Engine : public EventHandler
  {
  public:
    Engine();
    ~Engine();
    void Update(float aDt);

    GraphicsSystem mGraphicsSystem;
    AudioSystem mAudioSystem;

    std::vector<std::unique_ptr<Window>> mWindows;
    Window *mPrimaryWindow;

    bool mShouldUpdate;

    u64 mCurrentFrame = 0;
  };
}
#endif
