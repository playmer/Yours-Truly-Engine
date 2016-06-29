//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef YTE_GraphicsSystem_hpp
#define YTE_GraphicsSystem_hpp

#include <stddef.h>

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>

#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

#include "YTE/Core/PrivateImplementation.hpp"


namespace YTE
{
  class GraphicsSystem
  {
    public:
    GraphicsSystem(Engine *aEngine);
    ~GraphicsSystem();

    void SetUpWindow(Window *aWindow);

    void Initialize();

    void Update(float aDt);

    private:
    Engine *mEngine;

    PrivateImplementation<1024> mPlatformSpecificData;
  };

}
#endif
