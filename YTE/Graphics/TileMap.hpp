//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////

#pragma once
#ifndef YTE_TileMap_hpp
#define YTE_TileMap_hpp

#include <assert.h>

#include "YTE/Graphics/VulkanContext.hpp"

#include "vulkan/vkel.h"
#include "vulkan/vk_cpp.hpp"

#include "YTE/Core/Types.hpp"

#include "glm/glm.hpp"
#include "gtc/matrix_transform.hpp"


namespace YTE
{
  struct Tile
  {
    Tile(glm::i32vec2 aPosition, u32 aTile) : mPosition(aPosition), mTile(aTile) {};

    glm::i32vec2 mPosition;
    u32 mTile;
  };

  class TileMap
  {
    public:
    struct InstanceData 
    {
      glm::vec2 mPostion;
      u32 mTile;
    };


    struct InstanceBuffer
    {
      BufferMemory mBufferMemory;
      size_t mSize = 0;
      vk::DescriptorBufferInfo mDescriptor;
    };

    TileMap(std::vector<Tile> &aMap, std::vector<std::string> &aTextureFiles, VulkanContext *aContext);

    ~TileMap();

    void PrepareInstanceData();

    void SetupCommandBuffer();
    void Draw();

    private:
    std::vector<Tile> mMap;
    std::vector<Texture> mTextures;
    VulkanContext *mContext;
    BufferMemory mVertexMemory;
    BufferMemory mIndexMemory;

    InstanceBuffer mInstanceBuffer;

    UniformBufferObject mUniformObject;
    BufferMemory mUniformMemory;

    glm::vec3 mPosition = { 0.0f, 0.0f, 0.0f };
    glm::vec3 mScale = { 1.0f, 1.0f, 1.0f };
    glm::vec3 mRotation = { 0.0f, 0.0f, 0.0f };
  };
};

#endif
