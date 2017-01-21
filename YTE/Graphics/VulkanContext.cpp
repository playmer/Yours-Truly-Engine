#include "YTE/Graphics/VulkanContext.hpp"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

namespace YTE
{
  void VulkanContext::CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
  {
    vk::CommandBufferAllocateInfo allocInfo = {};
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandPool = mCommandPool;
    allocInfo.commandBufferCount = 1;

    auto commandBuffer = mLogicalDevice.allocateCommandBuffers(allocInfo)[0];

    vk::CommandBufferBeginInfo beginInfo = {};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    commandBuffer.begin(beginInfo);

    vk::BufferCopy copyRegion = {};
    copyRegion.size = size;
    commandBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);

    commandBuffer.end();

    vk::SubmitInfo submitInfo = {};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    mQueue.submit(submitInfo, VK_NULL_HANDLE);
    mQueue.waitIdle();

    mLogicalDevice.freeCommandBuffers(mCommandPool, commandBuffer);
  }

  BufferMemory VulkanContext::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties)
  {
    BufferMemory toReturn(&mLogicalDevice);

    vk::BufferCreateInfo bufferInfo = {};
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    toReturn.mBuffer = mLogicalDevice.createBuffer(bufferInfo);

    auto memReqs = mLogicalDevice.getBufferMemoryRequirements(toReturn.mBuffer);

    vk::MemoryAllocateInfo allocInfo = {};
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, properties);

    toReturn.mMemory = mLogicalDevice.allocateMemory(allocInfo);

    mLogicalDevice.bindBufferMemory(toReturn.mBuffer, toReturn.mMemory, 0);

    return toReturn;
  }
  
  void VulkanContext::SetupDescriptorPool()
  {
    // We need to tell the API the number of max. requested descriptors per type
    std::array<vk::DescriptorPoolSize, 2> typeCounts;

    typeCounts[0].type = vk::DescriptorType::eUniformBuffer;
    typeCounts[0].descriptorCount = 1;

    typeCounts[1].type = vk::DescriptorType::eCombinedImageSampler;
    typeCounts[1].descriptorCount = static_cast<u32>(mTextures.size()); // TODO: Make sure the layers don't yell.

    vk::DescriptorPoolCreateInfo descriptorPoolInfo = {};
    descriptorPoolInfo.poolSizeCount = static_cast<u32>(typeCounts.size());
    descriptorPoolInfo.pPoolSizes = typeCounts.data();

    // Set the max. number of sets that can be requested
    // Requesting descriptors beyond maxSets will result in an error
    descriptorPoolInfo.maxSets = 2;

    mDescriptorPool = mLogicalDevice.createDescriptorPool(descriptorPoolInfo);
  }

  void VulkanContext::SetupDescriptorSet()
  {
    mWriteDescriptorSet.resize(2); // Need at least 1 for the uniform buffer.

    // Allocate a new descriptor set from the global descriptor pool
    vk::DescriptorSetAllocateInfo allocInfo = {};
    allocInfo.descriptorPool = mDescriptorPool;

    std::vector<vk::DescriptorSetLayout> layouts;
    

    for (size_t i = 0; i < mMaterials.size(); ++i)
    {
      auto &material = mMaterials[i];
      layouts.emplace_back(material.mDescriptorSetLayout);
      material.mDescriptorSetOffset = static_cast<u32>(i);
    }

    allocInfo.descriptorSetCount = static_cast<u32>(layouts.size());
    allocInfo.pSetLayouts = layouts.data();


    mDescriptorSets = mLogicalDevice.allocateDescriptorSets(allocInfo);

    // Update the descriptor set determining the shader binding points
    // For every binding point used in a shader there needs to be one
    // descriptor set matching that binding point
    // Binding 0 : Uniform buffer
    mWriteDescriptorSet[0].dstSet = mDescriptorSets[0];
    mWriteDescriptorSet[0].descriptorCount = 1;
    mWriteDescriptorSet[0].descriptorType = vk::DescriptorType::eUniformBuffer;
    mWriteDescriptorSet[0].pBufferInfo = &mView.mUniformBufferInfo;
    mWriteDescriptorSet[0].dstBinding = 0;

    std::vector<vk::DescriptorImageInfo> imageInfos;
    imageInfos.reserve(mTextures.size());

    for (auto &texture : mTextures)
    {
      imageInfos.emplace_back(texture.mDescriptor);
    }

    mWriteDescriptorSet[1].dstSet = mDescriptorSets[0];
    mWriteDescriptorSet[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    mWriteDescriptorSet[1].dstBinding = 1;
    mWriteDescriptorSet[1].dstArrayElement = 0;
    mWriteDescriptorSet[1].pImageInfo = imageInfos.data();
    mWriteDescriptorSet[1].descriptorCount = static_cast<u32>(imageInfos.size());

    mLogicalDevice.updateDescriptorSets(mWriteDescriptorSet, VK_NULL_HANDLE);
  }


  void VulkanContext::UpdateDescriptorSet(Texture &aTexture)
  {
    //ImageDescriptor for the color map texture;
    vk::DescriptorImageInfo textureDescriptor;
    textureDescriptor.sampler = aTexture.mSampler;
    textureDescriptor.imageView = aTexture.mView;
    textureDescriptor.imageLayout = vk::ImageLayout::eGeneral; // NOTE: Always this currently.

    mWriteDescriptorSet[1].pImageInfo = &textureDescriptor;

    mLogicalDevice.updateDescriptorSets(mWriteDescriptorSet, VK_NULL_HANDLE);
  }
}