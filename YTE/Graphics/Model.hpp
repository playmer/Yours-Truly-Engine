#pragma once

#include <vector>

#include "glm.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/Instance.hpp"
#include "YTE/Graphics/VulkanRenderer.hpp"

namespace YTE
{
  // Data to be processed into InstanceDatas
  class Mesh
  {
  public:
    Mesh(std::initializer_list<Vertex> &aVertices, std::initializer_list<u32> &aIndicies, VulkanRenderer *aContext)
    {
      mVerticies = aContext->CreateFilledBuffer(aVertices.begin(), aVertices.size(), true);
      mIndicies  = aContext->CreateFilledBuffer(aIndicies.begin(), aIndicies.size(), true);
    }

    BufferMemory mVerticies;
    BufferMemory mIndicies;
  };

  class MeshInstance
  {
  public:
    //void DrawInstances();
    //void AddInstance();

    std::vector<Instance> mInstances;
    std::vector<InstanceData> mInstanceDatas;
    std::vector<size_t> mChangedInstances;

    // Mesh these instances are based on.
    Mesh *mMesh;

    // Where we store the instance data on the GPU.
    BufferMemory mInstanceDataBuffer;
    u32 mInstanceDataBufferSize = 0;
  };
}