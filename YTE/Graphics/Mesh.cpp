#include "YTE/Graphics/Mesh.hpp"

namespace YTE
{
  inline void Mesh::setupVertexInputState(std::vector<VertexLayout> layout)
  {
    mBindingDescription = { mVertexBufferBinding, vertexSize(layout), vk::VertexInputRate::eVertex };

    mAttributeDescriptions.clear();
    u32 offset = 0;
    u32 binding = 0;

    for (auto& layoutDetail : layout)
    {
      // Format (layout)
      vk::Format format = (layoutDetail == VertexLayout::VERTEX_LAYOUT_UV) ? vk::Format::eR32G32Sfloat : vk::Format::eR32G32B32Sfloat;

      mAttributeDescriptions.push_back({ mVertexBufferBinding,
                                       binding,
                                       format,
                                       offset });

      // Offset
      offset += (layoutDetail == VertexLayout::VERTEX_LAYOUT_UV) ? (2 * sizeof(float)) : (3 * sizeof(float));
      binding++;
    }

    mVertexInputState = vk::PipelineVertexInputStateCreateInfo();
    mVertexInputState.vertexBindingDescriptionCount = 1;
    mVertexInputState.pVertexBindingDescriptions = &mBindingDescription;
    mVertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(mAttributeDescriptions.size());
    mVertexInputState.pVertexAttributeDescriptions = mAttributeDescriptions.data();
  }
  
  void Mesh::drawIndexed(vk::CommandBuffer aCommandBuffer)
  {
    VkDeviceSize offsets[1] = { 0 };

    if (mPipeline)
    {
      aCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);
    }

    if (mPipelineLayout && mDescriptorSet)
    {
      aCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout, 0, mDescriptorSet, nullptr);
    }

    aCommandBuffer.bindVertexBuffers(mVertexBufferBinding, 1, &mBuffers.mVertices.mBuffer, offsets);
    aCommandBuffer.bindIndexBuffer(mBuffers.mIndices.mBuffer, 0, vk::IndexType::eUint32);
    aCommandBuffer.drawIndexed(mBuffers.mIndexCount, 1, 0, 0, 0);
  }

  void freeMeshBufferResources(vk::Device device, MeshBuffer * meshBuffer)
  {
    vkDestroyBuffer(device, meshBuffer->mVertices.mBuffer, nullptr);
    vkFreeMemory(device, meshBuffer->mVertices.mMemory, nullptr);

    if (meshBuffer->mIndices.mBuffer)
    {
      device.destroyBuffer(meshBuffer->mIndices.mBuffer);
      device.freeMemory(meshBuffer->mIndices.mMemory);
    }
  }

  // Get vertex size from vertex layout
  uint32_t vertexSize(std::vector<VertexLayout> layout)
  {
    uint32_t vSize = 0;

    for (auto& layoutDetail : layout)
    {
      switch (layoutDetail)
      {
        // UV only has two components
        case VertexLayout::VERTEX_LAYOUT_UV:
        {
          vSize += 2 * sizeof(float);
          break;
        }
        default:
        {
          vSize += 3 * sizeof(float);
        }
      }
    }

    return vSize;
  }
}