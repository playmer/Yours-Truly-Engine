//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////

#pragma once
#ifndef YTE_TextureLoader_hpp
#define YTE_TextureLoader_hpp

#include <assert.h>

#include "YTE/Core/Types.hpp"

#include "YTE/Graphics/Texture.hpp"

#include "vulkan/vkel.h"
#include "vulkan/vk_cpp.hpp"
#include "stb/stb_image.h"

namespace YTE
{
  template<typename FlagType>
  u32 GetMemoryType(u32 aTypeBits, vk::PhysicalDeviceMemoryProperties aDeviceProperties, FlagType aProperties)
  {
    for (uint32_t i = 0; i < 32; i++)
    {
      if ((aTypeBits & 1) == 1)
      {
        if ((aDeviceProperties.memoryTypes[i].propertyFlags & aProperties) == aProperties)
        {
          return i;
        }
      }
      aTypeBits >>= 1;
    }

    // todo : throw error
    return 0;
  }

  class TextureLoader
  {
    private:
    vk::PhysicalDevice physicalDevice;
    vk::Device device;
    vk::Queue queue;
    vk::CommandBuffer cmdBuffer;
    vk::CommandPool cmdPool;
    vk::PhysicalDeviceMemoryProperties deviceMemoryProperties;

    public:
    TextureLoader(vk::PhysicalDevice aPhysicalDevice, vk::Device aDevice, vk::Queue aQueue, vk::CommandPool aCommandPool);

    ~TextureLoader();

    // Create an image memory barrier for changing the layout of
    // an image and put it into an active command buffer
    void setImageLayout(vk::CommandBuffer cmdBuffer, vk::Image image, vk::ImageAspectFlags aspectMask, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout, vk::ImageSubresourceRange subresourceRange);
    
    // Fixed sub resource on first mip level and layer
    void setImageLayout(vk::CommandBuffer cmdbuffer, vk::Image image, vk::ImageAspectFlags aspectMask, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout);

    // Load a 2D texture
    Texture TextureLoader::loadTexture(const std::string &filename);

    // Clean up vulkan resources used by a texture object
    void destroyTexture(Texture texture);

    void createImageView(vk::Image aImage, vk::Format aFormat, vk::ImageView &aImageView);
    void createTextureImageView(Texture &aTexture);
    void createTextureSampler(Texture &aTexture);

    void createImage(uint32_t aWidth, uint32_t aHeight, vk::Format aFormat, vk::ImageTiling aTiling, vk::ImageUsageFlags aUsage, vk::MemoryPropertyFlags properties, vk::Image& aImage, vk::DeviceMemory &aImageMemory);

    void copyImage(vk::Image srcImage, vk::Image dstImage, u32 width, u32 height);
    vk::CommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(vk::CommandBuffer commandBuffer);
    void TextureLoader::transitionImageLayout(vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
    Texture TextureLoader::createTextureImage(const std::string &aTextureFile);

    vk::CommandBuffer createCommandBuffer(vk::CommandBufferLevel level, bool begin);

    //// Load a cubemap texture (single file)
    //void loadCubemap(std::string filename, vk::Format format, Texture *texture)
    //{
    //  gli::textureCube texCube(gli::load(filename));
    //  assert(!texCube.empty());
    //
    //  texture->width = static_cast<uint32_t>(texCube.dimensions().x);
    //  texture->height = static_cast<uint32_t>(texCube.dimensions().y);
    //  texture->mipLevels = static_cast<uint32_t>(texCube.levels());
    //
    //  vk::MemoryAllocateInfo memAllocInfo = vkTools::initializers::memoryAllocateInfo();
    //  vk::MemoryRequirements memReqs;
    //
    //  // Create a host-visible staging buffer that contains the raw image data
    //  vk::Buffer stagingBuffer;
    //  vk::DeviceMemory stagingMemory;
    //
    //  vk::BufferCreateInfo bufferCreateInfo = vkTools::initializers::bufferCreateInfo();
    //  bufferCreateInfo.size = texCube.size();
    //  // This buffer is used as a transfer source for the buffer copy
    //  bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    //  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    //
    //  VK_CHECK_RESULT(vkCreateBuffer(device, &bufferCreateInfo, nullptr, &stagingBuffer));
    //
    //  // Get memory requirements for the staging buffer (alignment, memory type bits)
    //  vkGetBufferMemoryRequirements(device, stagingBuffer, &memReqs);
    //
    //  memAllocInfo.allocationSize = memReqs.size;
    //  // Get memory type index for a host visible buffer
    //  memAllocInfo.memoryTypeIndex = getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    //
    //  VK_CHECK_RESULT(vkAllocateMemory(device, &memAllocInfo, nullptr, &stagingMemory));
    //  VK_CHECK_RESULT(vkBindBufferMemory(device, stagingBuffer, stagingMemory, 0));
    //
    //  // Copy texture data into staging buffer
    //  uint8_t *data;
    //  VK_CHECK_RESULT(vkMapMemory(device, stagingMemory, 0, memReqs.size, 0, (void **)&data));
    //  memcpy(data, texCube.data(), texCube.size());
    //  vkUnmapMemory(device, stagingMemory);
    //
    //  // Setup buffer copy regions for each face including all of it's miplevels
    //  std::vector<vk::BufferImageCopy> bufferCopyRegions;
    //  size_t offset = 0;
    //
    //  for (uint32_t face = 0; face < 6; face++)
    //  {
    //    for (uint32_t level = 0; level < texture->mipLevels; level++)
    //    {
    //      vk::BufferImageCopy bufferCopyRegion = {};
    //      bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //      bufferCopyRegion.imageSubresource.mipLevel = level;
    //      bufferCopyRegion.imageSubresource.baseArrayLayer = face;
    //      bufferCopyRegion.imageSubresource.layerCount = 1;
    //      bufferCopyRegion.imageExtent.width = static_cast<uint32_t>(texCube[face][level].dimensions().x);
    //      bufferCopyRegion.imageExtent.height = static_cast<uint32_t>(texCube[face][level].dimensions().y);
    //      bufferCopyRegion.imageExtent.depth = 1;
    //      bufferCopyRegion.bufferOffset = offset;
    //
    //      bufferCopyRegions.push_back(bufferCopyRegion);
    //
    //      // Increase offset into staging buffer for next level / face
    //      offset += texCube[face][level].size();
    //    }
    //  }
    //
    //  // Create optimal tiled target image
    //  vk::ImageCreateInfo imageCreateInfo = vkTools::initializers::imageCreateInfo();
    //  imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    //  imageCreateInfo.format = format;
    //  imageCreateInfo.mipLevels = texture->mipLevels;
    //  imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    //  imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    //  imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
    //  imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    //  imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //  imageCreateInfo.extent = { texture->width, texture->height, 1 };
    //  imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    //  // Cube faces count as array layers in Vulkan
    //  imageCreateInfo.arrayLayers = 6;
    //  // This flag is required for cube map images
    //  imageCreateInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    //
    //  VK_CHECK_RESULT(vkCreateImage(device, &imageCreateInfo, nullptr, &texture->image));
    //
    //  vkGetImageMemoryRequirements(device, texture->image, &memReqs);
    //
    //  memAllocInfo.allocationSize = memReqs.size;
    //  memAllocInfo.memoryTypeIndex = getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    //
    //  VK_CHECK_RESULT(vkAllocateMemory(device, &memAllocInfo, nullptr, &texture->deviceMemory));
    //  VK_CHECK_RESULT(vkBindImageMemory(device, texture->image, texture->deviceMemory, 0));
    //
    //  vk::CommandBufferBeginInfo cmdBufInfo = vkTools::initializers::commandBufferBeginInfo();
    //  VK_CHECK_RESULT(vkBeginCommandBuffer(cmdBuffer, &cmdBufInfo));
    //
    //  // Image barrier for optimal image (target)
    //  // Set initial layout for all array layers (faces) of the optimal (target) tiled texture
    //  vk::ImageSubresourceRange subresourceRange = {};
    //  subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //  subresourceRange.baseMipLevel = 0;
    //  subresourceRange.levelCount = texture->mipLevels;
    //  subresourceRange.layerCount = 6;
    //
    //  vkTools::setImageLayout(
    //    cmdBuffer,
    //    texture->image,
    //    VK_IMAGE_ASPECT_COLOR_BIT,
    //    VK_IMAGE_LAYOUT_UNDEFINED,
    //    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    //    subresourceRange);
    //
    //  // Copy the cube map faces from the staging buffer to the optimal tiled image
    //  vkCmdCopyBufferToImage(
    //    cmdBuffer,
    //    stagingBuffer,
    //    texture->image,
    //    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    //    static_cast<uint32_t>(bufferCopyRegions.size()),
    //    bufferCopyRegions.data());
    //
    //  // Change texture image layout to shader read after all faces have been copied
    //  texture->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    //  vkTools::setImageLayout(
    //    cmdBuffer,
    //    texture->image,
    //    VK_IMAGE_ASPECT_COLOR_BIT,
    //    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    //    texture->imageLayout,
    //    subresourceRange);
    //
    //  VK_CHECK_RESULT(vkEndCommandBuffer(cmdBuffer));
    //
    //  // Create a fence to make sure that the copies have finished before continuing
    //  vk::Fence copyFence;
    //  vk::FenceCreateInfo fenceCreateInfo = vkTools::initializers::fenceCreateInfo(VK_FLAGS_NONE);
    //  VK_CHECK_RESULT(vkCreateFence(device, &fenceCreateInfo, nullptr, &copyFence));
    //
    //  vk::SubmitInfo submitInfo = vkTools::initializers::submitInfo();
    //  submitInfo.commandBufferCount = 1;
    //  submitInfo.pCommandBuffers = &cmdBuffer;
    //
    //  VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submitInfo, copyFence));
    //
    //  VK_CHECK_RESULT(vkWaitForFences(device, 1, &copyFence, VK_TRUE, DEFAULT_FENCE_TIMEOUT));
    //
    //  vkDestroyFence(device, copyFence, nullptr);
    //
    //  // Create sampler
    //  vk::SamplerCreateInfo sampler = vkTools::initializers::samplerCreateInfo();
    //  sampler.magFilter = VK_FILTER_LINEAR;
    //  sampler.minFilter = VK_FILTER_LINEAR;
    //  sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    //  sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    //  sampler.addressModeV = sampler.addressModeU;
    //  sampler.addressModeW = sampler.addressModeU;
    //  sampler.mipLodBias = 0.0f;
    //  sampler.maxAnisotropy = 8;
    //  sampler.compareOp = VK_COMPARE_OP_NEVER;
    //  sampler.minLod = 0.0f;
    //  sampler.maxLod = (float)texture->mipLevels;
    //  sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    //  VK_CHECK_RESULT(vkCreateSampler(device, &sampler, nullptr, &texture->sampler));
    //
    //  // Create image view
    //  vk::ImageViewCreateInfo view = vkTools::initializers::imageViewCreateInfo();
    //  view.image = VK_NULL_HANDLE;
    //  view.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
    //  view.format = format;
    //  view.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
    //  view.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    //  view.subresourceRange.layerCount = 6;
    //  view.subresourceRange.levelCount = texture->mipLevels;
    //  view.image = texture->image;
    //  VK_CHECK_RESULT(vkCreateImageView(device, &view, nullptr, &texture->view));
    //
    //  // Clean up staging resources
    //  vkFreeMemory(device, stagingMemory, nullptr);
    //  vkDestroyBuffer(device, stagingBuffer, nullptr);
    //
    //  // Fill descriptor image info that can be used for setting up descriptor sets
    //  texture->descriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    //  texture->descriptor.imageView = texture->view;
    //  texture->descriptor.sampler = texture->sampler;
    //}

    //// Load an array texture (single file)
    //void loadTextureArray(std::string filename, vk::Format format, Texture *texture)
    //{
    //  gli::texture2DArray tex2DArray(gli::load(filename));
    //
    //  assert(!tex2DArray.empty());
    //
    //  texture->width = static_cast<uint32_t>(tex2DArray.dimensions().x);
    //  texture->height = static_cast<uint32_t>(tex2DArray.dimensions().y);
    //  texture->layerCount = static_cast<uint32_t>(tex2DArray.layers());
    //  texture->mipLevels = static_cast<uint32_t>(tex2DArray.levels());
    //
    //  vk::MemoryAllocateInfo memAllocInfo = vkTools::initializers::memoryAllocateInfo();
    //  vk::MemoryRequirements memReqs;
    //
    //  // Create a host-visible staging buffer that contains the raw image data
    //  vk::Buffer stagingBuffer;
    //  vk::DeviceMemory stagingMemory;
    //
    //  vk::BufferCreateInfo bufferCreateInfo = vkTools::initializers::bufferCreateInfo();
    //  bufferCreateInfo.size = tex2DArray.size();
    //  // This buffer is used as a transfer source for the buffer copy
    //  bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    //  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    //
    //  VK_CHECK_RESULT(vkCreateBuffer(device, &bufferCreateInfo, nullptr, &stagingBuffer));
    //
    //  // Get memory requirements for the staging buffer (alignment, memory type bits)
    //  vkGetBufferMemoryRequirements(device, stagingBuffer, &memReqs);
    //
    //  memAllocInfo.allocationSize = memReqs.size;
    //  // Get memory type index for a host visible buffer
    //  memAllocInfo.memoryTypeIndex = getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    //
    //  VK_CHECK_RESULT(vkAllocateMemory(device, &memAllocInfo, nullptr, &stagingMemory));
    //  VK_CHECK_RESULT(vkBindBufferMemory(device, stagingBuffer, stagingMemory, 0));
    //
    //  // Copy texture data into staging buffer
    //  uint8_t *data;
    //  VK_CHECK_RESULT(vkMapMemory(device, stagingMemory, 0, memReqs.size, 0, (void **)&data));
    //  memcpy(data, tex2DArray.data(), static_cast<size_t>(tex2DArray.size()));
    //  vkUnmapMemory(device, stagingMemory);
    //
    //  // Setup buffer copy regions for each layer including all of it's miplevels
    //  std::vector<vk::BufferImageCopy> bufferCopyRegions;
    //  size_t offset = 0;
    //
    //  for (uint32_t layer = 0; layer < texture->layerCount; layer++)
    //  {
    //    for (uint32_t level = 0; level < texture->mipLevels; level++)
    //    {
    //      vk::BufferImageCopy bufferCopyRegion = {};
    //      bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //      bufferCopyRegion.imageSubresource.mipLevel = level;
    //      bufferCopyRegion.imageSubresource.baseArrayLayer = layer;
    //      bufferCopyRegion.imageSubresource.layerCount = 1;
    //      bufferCopyRegion.imageExtent.width = static_cast<uint32_t>(tex2DArray[layer][level].dimensions().x);
    //      bufferCopyRegion.imageExtent.height = static_cast<uint32_t>(tex2DArray[layer][level].dimensions().y);
    //      bufferCopyRegion.imageExtent.depth = 1;
    //      bufferCopyRegion.bufferOffset = offset;
    //
    //      bufferCopyRegions.push_back(bufferCopyRegion);
    //
    //      // Increase offset into staging buffer for next level / face
    //      offset += tex2DArray[layer][level].size();
    //    }
    //  }
    //
    //  // Create optimal tiled target image
    //  vk::ImageCreateInfo imageCreateInfo = vkTools::initializers::imageCreateInfo();
    //  imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    //  imageCreateInfo.format = format;
    //  imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    //  imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    //  imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
    //  imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    //  imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //  imageCreateInfo.extent = { texture->width, texture->height, 1 };
    //  imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    //  imageCreateInfo.arrayLayers = texture->layerCount;
    //  imageCreateInfo.mipLevels = texture->mipLevels;
    //
    //  VK_CHECK_RESULT(vkCreateImage(device, &imageCreateInfo, nullptr, &texture->image));
    //
    //  vkGetImageMemoryRequirements(device, texture->image, &memReqs);
    //
    //  memAllocInfo.allocationSize = memReqs.size;
    //  memAllocInfo.memoryTypeIndex = getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    //
    //  VK_CHECK_RESULT(vkAllocateMemory(device, &memAllocInfo, nullptr, &texture->deviceMemory));
    //  VK_CHECK_RESULT(vkBindImageMemory(device, texture->image, texture->deviceMemory, 0));
    //
    //  vk::CommandBufferBeginInfo cmdBufInfo = vkTools::initializers::commandBufferBeginInfo();
    //  VK_CHECK_RESULT(vkBeginCommandBuffer(cmdBuffer, &cmdBufInfo));
    //
    //  // Image barrier for optimal image (target)
    //  // Set initial layout for all array layers (faces) of the optimal (target) tiled texture
    //  vk::ImageSubresourceRange subresourceRange = {};
    //  subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //  subresourceRange.baseMipLevel = 0;
    //  subresourceRange.levelCount = texture->mipLevels;
    //  subresourceRange.layerCount = texture->layerCount;
    //
    //  vkTools::setImageLayout(
    //    cmdBuffer,
    //    texture->image,
    //    VK_IMAGE_ASPECT_COLOR_BIT,
    //    VK_IMAGE_LAYOUT_UNDEFINED,
    //    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    //    subresourceRange);
    //
    //  // Copy the layers and mip levels from the staging buffer to the optimal tiled image
    //  vkCmdCopyBufferToImage(
    //    cmdBuffer,
    //    stagingBuffer,
    //    texture->image,
    //    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    //    static_cast<uint32_t>(bufferCopyRegions.size()),
    //    bufferCopyRegions.data());
    //
    //  // Change texture image layout to shader read after all faces have been copied
    //  texture->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    //  vkTools::setImageLayout(
    //    cmdBuffer,
    //    texture->image,
    //    VK_IMAGE_ASPECT_COLOR_BIT,
    //    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    //    texture->imageLayout,
    //    subresourceRange);
    //
    //  VK_CHECK_RESULT(vkEndCommandBuffer(cmdBuffer));
    //
    //  // Create a fence to make sure that the copies have finished before continuing
    //  vk::Fence copyFence;
    //  vk::FenceCreateInfo fenceCreateInfo = vkTools::initializers::fenceCreateInfo(VK_FLAGS_NONE);
    //  VK_CHECK_RESULT(vkCreateFence(device, &fenceCreateInfo, nullptr, &copyFence));
    //
    //  vk::SubmitInfo submitInfo = vkTools::initializers::submitInfo();
    //  submitInfo.commandBufferCount = 1;
    //  submitInfo.pCommandBuffers = &cmdBuffer;
    //
    //  VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submitInfo, copyFence));
    //
    //  VK_CHECK_RESULT(vkWaitForFences(device, 1, &copyFence, VK_TRUE, DEFAULT_FENCE_TIMEOUT));
    //
    //  vkDestroyFence(device, copyFence, nullptr);
    //
    //  // Create sampler
    //  vk::SamplerCreateInfo sampler = vkTools::initializers::samplerCreateInfo();
    //  sampler.magFilter = VK_FILTER_LINEAR;
    //  sampler.minFilter = VK_FILTER_LINEAR;
    //  sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    //  sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    //  sampler.addressModeV = sampler.addressModeU;
    //  sampler.addressModeW = sampler.addressModeU;
    //  sampler.mipLodBias = 0.0f;
    //  sampler.maxAnisotropy = 8;
    //  sampler.compareOp = VK_COMPARE_OP_NEVER;
    //  sampler.minLod = 0.0f;
    //  sampler.maxLod = (float)texture->mipLevels;
    //  sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    //  VK_CHECK_RESULT(vkCreateSampler(device, &sampler, nullptr, &texture->sampler));
    //
    //  // Create image view
    //  vk::ImageViewCreateInfo view = vkTools::initializers::imageViewCreateInfo();
    //  view.image = VK_NULL_HANDLE;
    //  view.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    //  view.format = format;
    //  view.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
    //  view.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    //  view.subresourceRange.layerCount = texture->layerCount;
    //  view.subresourceRange.levelCount = texture->mipLevels;
    //  view.image = texture->image;
    //  VK_CHECK_RESULT(vkCreateImageView(device, &view, nullptr, &texture->view));
    //
    //  // Clean up staging resources
    //  vkFreeMemory(device, stagingMemory, nullptr);
    //  vkDestroyBuffer(device, stagingBuffer, nullptr);
    //
    //  // Fill descriptor image info that can be used for setting up descriptor sets
    //  texture->descriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    //  texture->descriptor.imageView = texture->view;
    //  texture->descriptor.sampler = texture->sampler;
    //}
  };

};

#endif
