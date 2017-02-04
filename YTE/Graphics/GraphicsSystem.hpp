//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef YTE_GraphicsSystem_hpp
#define YTE_GraphicsSystem_hpp

#include <stddef.h>

#include <functional>

#include "YTE/Core/EventHandler.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/PrivateImplementation.hpp"
#include "YTE/Core/Types.hpp"

#include "YTE/Graphics/Instance.hpp"
#include "YTE/Graphics/Material.hpp"
#include "YTE/Graphics/Model.hpp"
#include "YTE/Graphics/View.hpp"
#include "YTE/Graphics/VulkanRenderer.hpp"
#include "YTE/Graphics/VulkanPrimitives.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

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
    std::vector<Material> mMaterials;
    //std::vector<Mesh> mDescriptorSets;

    PrivateImplementationDynamic mPlatformSpecificData;

    private:
    Engine *mEngine;


	  i32 mVulkanSuccess;
  };
}

#endif
