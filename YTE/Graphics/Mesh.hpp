//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef YTE_Mesh_hpp
#define YTE_Mesh_hpp

#include <vector>

#include "vulkan/vkel.h"
#include "vulkan/vk_cpp.hpp"

#include "YTE/Core/Types.hpp"
#include "YTE/Graphics/Vertex.hpp"

namespace YTE
{
  enum class VertexLayout 
  {
    VERTEX_LAYOUT_POSITION = 0x0,
    VERTEX_LAYOUT_NORMAL = 0x1,
    VERTEX_LAYOUT_COLOR = 0x2,
    VERTEX_LAYOUT_UV = 0x3,
    VERTEX_LAYOUT_TANGENT = 0x4,
    VERTEX_LAYOUT_BITANGENT = 0x5,
    VERTEX_LAYOUT_DUMMY_FLOAT = 0x6,
    VERTEX_LAYOUT_DUMMY_VEC4 = 0x7
  };

  struct MeshBufferInfo
  {
    vk::Buffer mBuffer = VK_NULL_HANDLE;
    vk::DeviceMemory mMemory = VK_NULL_HANDLE;
    size_t size = 0;
  };

  struct MeshBuffer
  {
    MeshBufferInfo mVertices;
    MeshBufferInfo mIndices;
    uint32_t mIndexCount;
    glm::vec3 mDim;
  };

  // Get vertex size from vertex layout
  u32 vertexSize(std::vector<VertexLayout> layout);
  void freeMeshBufferResources(vk::Device device, MeshBuffer * meshBuffer);

  // Stores some additonal info and functions for 
  // specifying pipelines, vertex bindings, etc.
  class Mesh
  {
    public:
    MeshBuffer mBuffers;

    vk::PipelineLayout mPipelineLayout = VK_NULL_HANDLE;
    vk::Pipeline mPipeline = VK_NULL_HANDLE;
    vk::DescriptorSet mDescriptorSet = VK_NULL_HANDLE;

    u32 mVertexBufferBinding = 0;

    vk::PipelineVertexInputStateCreateInfo mVertexInputState;
    vk::VertexInputBindingDescription mBindingDescription;
    std::vector<vk::VertexInputAttributeDescription> mAttributeDescriptions;

    void setupVertexInputState(std::vector<VertexLayout> layout);

    void drawIndexed(vk::CommandBuffer aCommandBuffer);
  };
}

#endif