//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef YTE_GraphicsSystem_hpp
#define YTE_GraphicsSystem_hpp

#include <stddef.h>

#include "YTE/Core/EventHandler.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Types.hpp"

#include "YTE/Graphics/Instance.hpp"
#include "YTE/Graphics/Model.hpp"
#include "YTE/Graphics/VulkanContext.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

#include "YTE/Core/PrivateImplementation.hpp"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

#include "vulkan/vkel.h"
#include "vulkan/vk_cpp.hpp"

namespace YTE
{
  class GraphicsSystem;


  class GraphicsSystem : public EventHandler
  {
    public:
    GraphicsSystem(Engine *aEngine);
    ~GraphicsSystem();

    void SetUpWindow(Window *aWindow);

    void Initialize();

    void Update(LogicUpdate *aUpdate);
    void VulkanRender();

    void SetupDrawing();

    void SetupInstanceDataBuffer();

    Quad *mQuad;

    YTE::Quad mBaseQuad;
    glm::i16vec2 mMousePosition = { 0,0 };

    std::vector<Mesh> mMeshes;
    //std::vector<Mesh> mDescriptorSets;

    PrivateImplementation<4096> mPlatformSpecificData;

    private:
    Engine *mEngine;


	  i32 mVulkanSuccess;
  };

}
#endif
