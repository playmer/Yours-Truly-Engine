//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef YTE_GraphicsSystem_hpp
#define YTE_GraphicsSystem_hpp

#include <stddef.h>

#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Types.hpp"

#include "YTE/Graphics/VulkanContext.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

#include "YTE/Core/PrivateImplementation.hpp"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

#include "vulkan/vkel.h"
#include "vulkan/vk_cpp.hpp"

namespace YTE
{
  struct Object
  {
    glm::vec3 mTranslation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 mScale = { 1.0f, 1.0f, 1.0f };
    glm::vec3 mRotate = { 0.0f, 0.0f, 0.0f };

    glm::vec3 mColor = { -1.0f, -1.0f, -1.0f };

    u32 mTextureId = 0;
  };

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

    void SetupObjectBuffer();

    Quad *mQuad;

    YTE::Quad mBaseQuad;
    glm::i16vec2 mMousePosition = { 0,0 };

    std::vector<Object> mObjects;

    PrivateImplementation<4096> mPlatformSpecificData;

    private:
    Engine *mEngine;

    BufferMemory mQuadVerticies;
    BufferMemory mQuadIndicies;

    BufferMemory mObjectsBuffer;

    u32 mObjectsBufferSize = 0;

	  i32 mVulkanSuccess;
  };

}
#endif
