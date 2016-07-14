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
#include "YTE/Core/Types.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

#include "YTE/Core/PrivateImplementation.hpp"

#include "glm.hpp"

namespace YTE
{
  struct Vertex
  {
    glm::vec4 mPosition;
    glm::vec4 mColor;
  };

  struct Triangle
  {
    Vertex m1;
    Vertex m2;
    Vertex m3;
  };

  struct Quad
  {
    Triangle m1;
    Triangle m2;
  };

  class GraphicsSystem
  {
    public:
    GraphicsSystem(Engine *aEngine);
    ~GraphicsSystem();

    void SetUpWindow(Window *aWindow);

    void Initialize();

    void Update(float aDt);
    void VulkanRender();
    void UpdateUniformBuffers();
    void SetupDescriptorSetLayout();
    void SetupDescriptorSet();
    void SetupDescriptorPool();

    Quad *mQuad;
    glm::i16vec2 mMousePosition = { 0,0 };

    private:
    Engine *mEngine;

    PrivateImplementation<2048> mPlatformSpecificData;

	  i32 mVulkanSuccess;
  };

}
#endif
