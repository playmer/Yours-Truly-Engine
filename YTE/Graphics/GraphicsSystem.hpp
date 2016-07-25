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
#include "YTE/Core/PrivateImplementation.hpp"
#include "YTE/Core/Types.hpp"


#include "YTE/Graphics/TileMap.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

#include "vulkan/vkel.h"
#include "vulkan/vk_cpp.hpp"

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
    void VulkanRender();

    void CreateTileMap(std::vector<Tile> aMap, std::vector<std::string> aTextureFiles);

    Quad *mQuad;
    glm::i16vec2 mMousePosition = { 0,0 };

    //private:
    Engine *mEngine;

    PrivateImplementation<4096> mPlatformSpecificData;

    std::vector<TileMap> mTileMaps;

	  i32 mVulkanSuccess;
  };

}
#endif
