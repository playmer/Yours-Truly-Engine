//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef YTE_MeshLoader_hpp
#define YTE_MeshLoader_hpp

#include <vector>

#include "vulkan/vkel.h"
#include "vulkan/vk_cpp.hpp"

#include "include/assimp/Importer.hpp"
#include "include/assimp/postprocess.h"
#include "include/assimp/scene.h"

#include "glm.hpp"

#include "YTE/Core/Types.hpp"

#include "YTE/Graphics/Mesh.hpp"
#include "YTE/Graphics/Vertex.hpp"

namespace YTE
{
  // Simple mesh class for getting all the necessary stuff from models loaded via ASSIMP
  class MeshLoader
  {
    private:

    struct MeshEntry 
    {
      uint32_t NumIndices;
      uint32_t MaterialIndex;
      uint32_t vertexBase;
      std::vector<RealVertex> Vertices;
      std::vector<unsigned int> Indices;
    };

    public:
    std::vector<MeshEntry> mEntries;

    struct Dimension
    {
      glm::vec3 min = glm::vec3(FLT_MAX);
      glm::vec3 max = glm::vec3(-FLT_MAX);
      glm::vec3 size;
    };


    // Optional
    struct Buffer 
    {
      vk::Buffer mBuffer;
      vk::DeviceMemory mMemory;
    };
    using VertexBuffer = Buffer;

    struct IndexBuffer : Buffer
    {
      u32 count;
    };

    Dimension mDimension;
    VertexBuffer mVertexBuffer;
    IndexBuffer mIndexBuffer;

    u32 numVertices = 0;

    vk::PipelineVertexInputStateCreateInfo mVertexInputStateCreateInfo;
    std::vector<vk::VertexInputBindingDescription> mBindingDescriptions;
    std::vector<vk::VertexInputAttributeDescription> mAttributeDescriptions;
    vk::Pipeline mPipeline;

    Assimp::Importer Importer;
    const aiScene* pScene;

    ~MeshLoader()
    {
      mEntries.clear();
    }

    // Loads the mesh with some default flags
    bool LoadMesh(const std::string& filename);

    // Load the mesh with custom flags
    bool LoadMesh(const std::string& filename, int flags);

    bool InitFromScene(const aiScene* pScene, const std::string& Filename);

    void InitMesh(unsigned int index, const aiMesh* paiMesh, const aiScene* pScene);

    // Clean up vulkan resources used by a mesh
    static void freeVulkanResources(VkDevice device, MeshLoader *mesh)
    {
      vkDestroyBuffer(device, mesh->mVertexBuffer.mBuffer, nullptr);
      vkFreeMemory(device, mesh->mVertexBuffer.mMemory, nullptr);
      vkDestroyBuffer(device, mesh->mIndexBuffer.mBuffer, nullptr);
      vkFreeMemory(device, mesh->mIndexBuffer.mMemory, nullptr);
    }

    vk::Result createBuffer(
      vk::Device device,
      vk::PhysicalDeviceMemoryProperties deviceMemoryProperties,
      vk::BufferUsageFlags usageFlags,
      vk::MemoryPropertyFlags memoryPropertyFlags,
      vk::DeviceSize size,
      vk::Buffer *buffer,
      vk::DeviceMemory *memory);

    // Create vertex and index buffer with given layout
    // Note : Only does staging if a valid command buffer and transfer queue are passed
    void createBuffers(
      vk::Device device,
      vk::PhysicalDeviceMemoryProperties deviceMemoryProperties,
      MeshBuffer *meshBuffer,
      std::vector<VertexLayout> layout,
      float scale,
      bool useStaging,
      vk::CommandBuffer copyCmd,
      vk::Queue copyQueue);

    // Create vertex and index buffer with given layout
    void createVulkanBuffers(
      vk::Device device,
      vk::PhysicalDeviceMemoryProperties deviceMemoryProperties,
      MeshBuffer *meshBuffer,
      std::vector<VertexLayout> layout,
      float scale);
  };
}

#endif