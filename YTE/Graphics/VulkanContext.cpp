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

  BufferMemory VulkanContext::CreateVertexBuffer(std::vector<Vertex> aVertices, bool aUseStaging)
  {
    vk::DeviceSize bufferSize = sizeof(Vertex) * aVertices.size();

    BufferMemory toReturn;

    auto bufferMemory = CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    void *data = mLogicalDevice.mapMemory(bufferMemory.mMemory, 0, bufferSize);
    memcpy(data, aVertices.data(), (size_t)bufferSize);
    mLogicalDevice.unmapMemory(bufferMemory.mMemory);

    if (aUseStaging)
    {
      auto staging = CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);

      CopyBuffer(bufferMemory.mBuffer, staging.mBuffer, bufferSize);

      toReturn = staging;
    }
    else
    {
      toReturn = bufferMemory;
    }

    return toReturn;
  }

  BufferMemory VulkanContext::CreateIndexBuffer(std::vector<u32> aIndices, bool aUseStaging)
  {
    vk::DeviceSize bufferSize = sizeof(u32) * aIndices.size();

    BufferMemory toReturn;

    auto bufferMemory = CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    void *data = mLogicalDevice.mapMemory(bufferMemory.mMemory, 0, bufferSize);
    memcpy(data, aIndices.data(), (size_t)bufferSize);
    mLogicalDevice.unmapMemory(bufferMemory.mMemory);

    if (aUseStaging)
    {
      auto staging = CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);

      CopyBuffer(bufferMemory.mBuffer, staging.mBuffer, bufferSize);

      toReturn = staging;
    }
    else
    {
      toReturn = bufferMemory;
    }

    return toReturn;
  }

  BufferMemory VulkanContext::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties)
  {
    BufferMemory toReturn;

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
  
  void VulkanContext::UpdateUniformBuffers(bool aRecalculation)
  {
    if (aRecalculation)
    {
      // Update matrices
      mUniformBufferData.mProjectionMatrix = glm::perspective(glm::radians(60.0f), (float)mWidth / (float)mHeight, 0.1f, 256.0f);

      auto viewMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, mZoom));

      auto test = glm::scale(glm::mat4(), glm::vec3());

      mUniformBufferData.mModelMatrix = viewMatrix * glm::translate(glm::mat4(), mCameraPosition);
      mUniformBufferData.mModelMatrix = glm::rotate(mUniformBufferData.mModelMatrix, glm::radians(mRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
      mUniformBufferData.mModelMatrix = glm::rotate(mUniformBufferData.mModelMatrix, glm::radians(mRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
      mUniformBufferData.mModelMatrix = glm::rotate(mUniformBufferData.mModelMatrix, glm::radians(mRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

      mUniformBufferData.mViewPosition = glm::vec4(0.0f, 0.0f, -mZoom, 0.0f);
    }

    // Map uniform buffer and update it

    u8 *data = (u8*)mLogicalDevice.mapMemory(mUniformBufferMemory, 0, sizeof(UniformBufferObject));
    memcpy(data, &mUniformBufferData, sizeof(UniformBufferObject));
    mLogicalDevice.unmapMemory(mUniformBufferMemory);
  }
  
  void VulkanContext::SetupDescriptorSetLayout()
  {
    // Setup layout of descriptors used in this example
    // Basically connects the different shader stages to descriptors
    // for binding uniform buffers, image samplers, etc.
    // So every shader binding should map to one descriptor set layout
    // binding

    // Binding 0 : Uniform buffer (Vertex shader)

    //std::vector<vk::DescriptorSetLayoutBinding> layoutBinding;
    //layoutBinding.resize(mTextures.size() + 1);

    std::array<vk::DescriptorSetLayoutBinding, 2> layoutBinding;

    layoutBinding[0].descriptorCount = 1;
    layoutBinding[0].binding = 0;
    layoutBinding[0].stageFlags = vk::ShaderStageFlagBits::eVertex;
    layoutBinding[0].descriptorType = vk::DescriptorType::eUniformBuffer;

    layoutBinding[1].descriptorCount = static_cast<u32>(mTextures.size());
    layoutBinding[1].binding = 1;
    layoutBinding[1].stageFlags = vk::ShaderStageFlagBits::eFragment;
    layoutBinding[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;

    vk::DescriptorSetLayoutCreateInfo descriptorLayout = {};
    descriptorLayout.pNext = nullptr;
    descriptorLayout.bindingCount = static_cast<u32>(layoutBinding.size());
    descriptorLayout.pBindings = layoutBinding.data();

    mDescriptorSetLayout = mLogicalDevice.createDescriptorSetLayout(descriptorLayout);

    // Create the pipeline layout that is used to generate the rendering pipelines that
    // are based on this descriptor set layout
    // In a more complex scenario you would have different pipeline layouts for different
    // descriptor set layouts that could be reused
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &mDescriptorSetLayout;

    mPipelineLayout = mLogicalDevice.createPipelineLayout(pipelineLayoutCreateInfo);
  }
  
  void VulkanContext::SetupDescriptorPool()
  {
    // We need to tell the API the number of max. requested descriptors per type
    static vk::DescriptorPoolSize typeCounts[2];

    typeCounts[0].type = vk::DescriptorType::eUniformBuffer;
    typeCounts[0].descriptorCount = 1;

    typeCounts[1].type = vk::DescriptorType::eCombinedImageSampler;
    typeCounts[1].descriptorCount = static_cast<u32>(mTextures.size()); // TODO: Make sure the layers don't yell.

                                       // Create the global descriptor pool
                                       // All descriptors used in this example are allocated from this pool
    vk::DescriptorPoolCreateInfo descriptorPoolInfo = {};
    descriptorPoolInfo.poolSizeCount = 2;
    descriptorPoolInfo.pPoolSizes = typeCounts;

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
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &mDescriptorSetLayout;

    mDescriptorSets = mLogicalDevice.allocateDescriptorSets(allocInfo);



    // Update the descriptor set determining the shader binding points
    // For every binding point used in a shader there needs to be one
    // descriptor set matching that binding point
    // Binding 0 : Uniform buffer
    mWriteDescriptorSet[0].dstSet = mDescriptorSets[0];
    mWriteDescriptorSet[0].descriptorCount = 1;
    mWriteDescriptorSet[0].descriptorType = vk::DescriptorType::eUniformBuffer;
    mWriteDescriptorSet[0].pBufferInfo = &mUniformBufferInfo;
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