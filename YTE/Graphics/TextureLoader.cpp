#include "YTE/Graphics/TextureLoader.hpp"

// Create an image memory barrier for changing the layout of
// an image and put it into an active command buffer

namespace YTE
{
  TextureLoader::TextureLoader(vk::PhysicalDevice aPhysicalDevice, vk::Device aDevice, vk::Queue aQueue, vk::CommandPool aCommandPool)
  {
    physicalDevice = aPhysicalDevice;
    device = aDevice;
    queue = aQueue;
    cmdPool = aCommandPool;
    deviceMemoryProperties = physicalDevice.getMemoryProperties();

    // Create command buffer for submitting image barriers
    // and converting tilings
    vk::CommandBufferAllocateInfo cmdBufInfo = {};
    cmdBufInfo.commandPool = cmdPool;
    cmdBufInfo.level = vk::CommandBufferLevel::ePrimary;
    cmdBufInfo.commandBufferCount = 1;

    cmdBuffer = device.allocateCommandBuffers(cmdBufInfo)[0];
  }

  TextureLoader::~TextureLoader()
  {
    device.freeCommandBuffers(cmdPool, cmdBuffer);
  }

  void TextureLoader::setImageLayout(vk::CommandBuffer cmdBuffer, vk::Image image, vk::ImageAspectFlags aspectMask, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout, vk::ImageSubresourceRange subresourceRange)
  {
    // Create an image barrier object
    vk::ImageMemoryBarrier imageMemoryBarrier;
    imageMemoryBarrier.oldLayout = oldImageLayout;
    imageMemoryBarrier.newLayout = newImageLayout;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.subresourceRange = subresourceRange;

    // Only sets masks for layouts used in this example
    // For a more complete version that can be used with other layouts see vkTools::setImageLayout

    // Source layouts (old)
    switch (oldImageLayout)
    {
      case vk::ImageLayout::eUndefined:
      {
        // Only valid as initial layout, memory contents are not preserved
        // Can be accessed directly, no source dependency required
        imageMemoryBarrier.srcAccessMask = vk::AccessFlags();
        break;
      }
      case vk::ImageLayout::ePreinitialized:
      {
        // Only valid as initial layout for linear images, preserves memory contents
        // Make sure host writes to the image have been finished
        imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
        break;
      }
      case vk::ImageLayout::eTransferDstOptimal:
      {
        // Old layout is transfer destination
        // Make sure any writes to the image have been finished
        imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        break;
      }
    }

    // Target layouts (new)
    switch (newImageLayout)
    {
      case vk::ImageLayout::eTransferSrcOptimal:
      {
        // Transfer source (copy, blit)
        // Make sure any reads from the image have been finished
        imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
        break;
      }
      case vk::ImageLayout::eTransferDstOptimal:
      {
        // Transfer destination (copy, blit)
        // Make sure any writes to the image have been finished
        imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
        break;
      }
      case vk::ImageLayout::eShaderReadOnlyOptimal:
      {
        // Shader read (sampler, input attachment)
        imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        break;
      }
    }

    // Put barrier on top of pipeline
    vk::PipelineStageFlags srcStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;
    vk::PipelineStageFlags destStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;

    cmdBuffer.pipelineBarrier(srcStageFlags, destStageFlags, vk::DependencyFlags(), nullptr, nullptr, imageMemoryBarrier);
  }

  // Fixed sub resource on first mip level and layer

  void TextureLoader::setImageLayout(vk::CommandBuffer cmdbuffer, vk::Image image, vk::ImageAspectFlags aspectMask, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout)
  {
    vk::ImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = aspectMask;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.layerCount = 1;
    setImageLayout(cmdbuffer, image, aspectMask, oldImageLayout, newImageLayout, subresourceRange);
  }

  // Load a 2D texture

  void TextureLoader::loadTexture(std::string filename, vk::Format format, Texture * texture)
  {
    loadTexture(filename, format, texture, false);
  }

  // Load a 2D texture

  void TextureLoader::loadTexture(std::string aFilename, vk::Format aFormat, Texture * aTexture, bool aForceLinear)
  {
    loadTexture(aFilename, aFormat, aTexture, aForceLinear, vk::ImageUsageFlagBits::eSampled);
  }

  // Load a 2D texture

  void TextureLoader::loadTexture(std::string filename, vk::Format format, Texture *aTexture, bool forceLinear, vk::ImageUsageFlags imageUsageFlags)
  {
    // We pass 4 into stbi_load because we always want RGBA, even if the image only has RGB.
    int width, height, components;
    auto textureData = stbi_load(filename.c_str(), &width, &height, &components, 4);

    assert(textureData != nullptr);
    assert(width > 0);
    assert(height > 0);

    aTexture->width = width;
    aTexture->height = height;
    aTexture->mipLevels = 1; // TODO: Get MipLevels static_cast<uint32_t>(tex2D.levels());

    auto textureSize = width * height * (sizeof(u8) * 4); // Last bit is RGBA

                                                           // Get device properites for the requested texture format
    auto formatProperties = physicalDevice.getFormatProperties(format);

    // Only use linear tiling if requested (and supported by the device)
    // Support for linear tiling is mostly limited, so prefer to use
    // optimal tiling instead
    // On most implementations linear tiling will only support a very
    // limited amount of formats and features (mip maps, cubemaps, arrays, etc.)
    vk::Bool32 useStaging = !forceLinear;

    vk::MemoryAllocateInfo memAllocInfo;

    // Use a separate command buffer for texture loading
    vk::CommandBufferBeginInfo cmdBufInfo;
    cmdBuffer.begin(cmdBufInfo);

    if (useStaging)
    {
      vk::BufferCreateInfo bufferCreateInfo;
      bufferCreateInfo.size = textureSize;

      // This buffer is used as a transfer source for the buffer copy
      bufferCreateInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
      bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive; // TODO: Learn to not be exclusive.

      auto stagingBuffer = device.createBuffer(bufferCreateInfo);

      // Get memory requirements for the staging buffer (alignment, memory type bits)
      auto memReqs = device.getBufferMemoryRequirements(stagingBuffer);

      memAllocInfo.allocationSize = memReqs.size;
      // Get memory type index for a host visible buffer
      memAllocInfo.memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, deviceMemoryProperties, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

      auto stagingMemory = device.allocateMemory(memAllocInfo);
      device.bindBufferMemory(stagingBuffer, stagingMemory, 0);

      // Copy texture data into staging buffer
      auto data = (u8*)device.mapMemory(stagingMemory, 0, memReqs.size);
      memcpy(data, textureData, textureSize);
      device.unmapMemory(stagingMemory);


      // TODO: Figure out how to deal with MipMaps
      // Setup buffer copy regions for each mip level
      //std::vector<vk::BufferImageCopy> bufferCopyRegions;
      //uint32_t offset = 0;

      //for (uint32_t i = 0; i < texture->mipLevels; i++)
      //{
      //  vk::BufferImageCopy bufferCopyRegion;
      //  bufferCopyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
      //  bufferCopyRegion.imageSubresource.mipLevel = i;
      //  bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
      //  bufferCopyRegion.imageSubresource.layerCount = 1;
      //  bufferCopyRegion.imageExtent.width = static_cast<uint32_t>(tex2D[i].dimensions().x);
      //  bufferCopyRegion.imageExtent.height = static_cast<uint32_t>(tex2D[i].dimensions().y);
      //  bufferCopyRegion.imageExtent.depth = 1;
      //  bufferCopyRegion.bufferOffset = offset;
      //
      //  bufferCopyRegions.push_back(bufferCopyRegion);
      //
      //  offset += static_cast<uint32_t>(tex2D[i].size());
      //}

      vk::BufferImageCopy bufferCopyRegion;
      bufferCopyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
      bufferCopyRegion.imageSubresource.mipLevel = 1;
      bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
      bufferCopyRegion.imageSubresource.layerCount = 1;
      bufferCopyRegion.imageExtent.width = width;
      bufferCopyRegion.imageExtent.height = height;
      bufferCopyRegion.imageExtent.depth = 1;
      bufferCopyRegion.bufferOffset = 0;

      // Create optimal tiled target image
      vk::ImageCreateInfo imageCreateInfo;
      imageCreateInfo.imageType = vk::ImageType::e2D;
      imageCreateInfo.format = format;
      imageCreateInfo.mipLevels = aTexture->mipLevels;
      imageCreateInfo.arrayLayers = 1;
      imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
      imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
      imageCreateInfo.usage = imageUsageFlags;
      imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
      imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
      imageCreateInfo.extent = { aTexture->width, aTexture->height, 1 };
      imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;

      aTexture->image = device.createImage(imageCreateInfo);

      memReqs = device.getImageMemoryRequirements(aTexture->image);

      memAllocInfo.allocationSize = memReqs.size;

      memAllocInfo.memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, deviceMemoryProperties, vk::MemoryPropertyFlagBits::eDeviceLocal);
      aTexture->deviceMemory = device.allocateMemory(memAllocInfo);
      device.bindImageMemory(aTexture->image, aTexture->deviceMemory, 0);

      vk::ImageSubresourceRange subresourceRange = {};
      subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      subresourceRange.baseMipLevel = 0;
      subresourceRange.levelCount = aTexture->mipLevels;
      subresourceRange.layerCount = 1;

      // Image barrier for optimal image (target)
      // Optimal image will be used as destination for the copy
      setImageLayout(
        cmdBuffer,
        aTexture->image,
        vk::ImageAspectFlagBits::eColor,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal,
        subresourceRange);

      // Copy mip levels from staging buffer
      cmdBuffer.copyBufferToImage(stagingBuffer, aTexture->image, vk::ImageLayout::eTransferDstOptimal, bufferCopyRegion);

      // Change texture image layout to shader read after all mip levels have been copied
      aTexture->imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      setImageLayout(cmdBuffer, aTexture->image, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eTransferDstOptimal, aTexture->imageLayout, subresourceRange);

      // Submit command buffer containing copy and image layout commands
      cmdBuffer.end();

      // Create a fence to make sure that the copies have finished before continuing
      auto copyFence = device.createFence(vk::FenceCreateInfo());

      vk::SubmitInfo submitInfo;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = &cmdBuffer;

      queue.submit(submitInfo, copyFence);

      device.waitForFences(copyFence, true, UINT64_MAX);

      vkDestroyFence(device, copyFence, nullptr);

      // Clean up staging resources
      vkFreeMemory(device, stagingMemory, nullptr);
      vkDestroyBuffer(device, stagingBuffer, nullptr);
    }
    else
    {
      // Prefer using optimal tiling, as linear tiling 
      // may support only a small set of features 
      // depending on implementation (e.g. no mip maps, only one layer, etc.)

      // Check if this support is supported for linear tiling
      assert(formatProperties.linearTilingFeatures & vk::FormatFeatureFlagBits::eSampledImage);

      vk::ImageCreateInfo imageCreateInfo;
      imageCreateInfo.imageType = vk::ImageType::e2D;
      imageCreateInfo.format = format;
      imageCreateInfo.extent = { aTexture->width, aTexture->height, 1 };
      imageCreateInfo.mipLevels = 1;
      imageCreateInfo.arrayLayers = 1;
      imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
      imageCreateInfo.tiling = vk::ImageTiling::eLinear;
      imageCreateInfo.usage = imageUsageFlags;
      imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
      imageCreateInfo.initialLayout = vk::ImageLayout::ePreinitialized;

      // Load mip map level 0 to linear tiling image
      auto mappableImage = device.createImage(imageCreateInfo);

      // Get memory requirements for this image 
      // like size and alignment
      auto memReqs = device.getImageMemoryRequirements(mappableImage);

      // Set memory allocation size to required memory size
      memAllocInfo.allocationSize = memReqs.size;

      // Get memory type that can be mapped to host memory
      memAllocInfo.memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, deviceMemoryProperties, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

      // Allocate host memory
      auto mappableMemory = device.allocateMemory(memAllocInfo);

      // Bind allocated image for use
      device.bindImageMemory(mappableImage, mappableMemory, 0);

      // Get sub resource layout
      // Mip map count, array layer, etc.
      vk::ImageSubresource subRes = {};
      subRes.aspectMask = vk::ImageAspectFlagBits::eColor;
      subRes.mipLevel = 0;

      void *data;

      // Get sub resources layout 
      // Includes row pitch, size offsets, etc.
      auto subResLayout = device.getImageSubresourceLayout(mappableImage, subRes);

      // Map image memory
      data = device.mapMemory(mappableMemory, 0, memReqs.size);

      // Copy image data into memory
      memcpy(data, textureData, textureSize);

      device.unmapMemory(mappableMemory);

      // Linear tiled images don't need to be staged
      // and can be directly used as textures
      aTexture->image = mappableImage;
      aTexture->deviceMemory = mappableMemory;
      aTexture->imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

      // Setup image memory barrier
      setImageLayout(cmdBuffer, aTexture->image, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::ePreinitialized, aTexture->imageLayout);

      // Submit command buffer containing copy and image layout commands
      cmdBuffer.end();

      vk::SubmitInfo submitInfo{};
      submitInfo.waitSemaphoreCount = 0;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = &cmdBuffer;

      queue.submit(submitInfo, nullptr);
      queue.waitIdle();
    }

    // Create sampler
    vk::SamplerCreateInfo sampler = {};
    sampler.magFilter = vk::Filter::eLinear;
    sampler.minFilter = vk::Filter::eLinear;
    sampler.mipmapMode = vk::SamplerMipmapMode::eLinear;
    sampler.addressModeU = vk::SamplerAddressMode::eRepeat;
    sampler.addressModeV = vk::SamplerAddressMode::eRepeat;
    sampler.addressModeW = vk::SamplerAddressMode::eRepeat;
    sampler.mipLodBias = 0.0f;
    sampler.compareOp = vk::CompareOp::eNever;
    sampler.minLod = 0.0f;
    // Max level-of-detail should match mip level count
    sampler.maxLod = (useStaging) ? (float)aTexture->mipLevels : 0.0f;
    // Enable anisotropic filtering
    sampler.maxAnisotropy = 8;
    sampler.anisotropyEnable = VK_TRUE;
    sampler.borderColor = vk::BorderColor::eFloatOpaqueWhite;
    aTexture->sampler = device.createSampler(sampler);

    // Create image view
    // Textures are not directly accessed by the shaders and
    // are abstracted by image views containing additional
    // information and sub resource ranges
    vk::ImageViewCreateInfo view = {};
    view.viewType = vk::ImageViewType::e2D;
    view.format = format;
    view.components = { vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA };
    view.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

    // Linear tiling usually won't support mip maps
    // Only set mip map count if optimal tiling is used
    view.subresourceRange.levelCount = (useStaging) ? aTexture->mipLevels : 1;
    view.image = aTexture->image;
    aTexture->view = device.createImageView(view);

    // Fill descriptor image info that can be used for setting up descriptor sets
    aTexture->descriptor.imageLayout = vk::ImageLayout::eGeneral;
    aTexture->descriptor.imageView = aTexture->view;
    aTexture->descriptor.sampler = aTexture->sampler;
  }

  // Clean up vulkan resources used by a texture object

  void TextureLoader::destroyTexture(Texture texture)
  {
    device.destroyImageView(texture.view, nullptr);
    device.destroyImage(texture.image, nullptr);
    device.destroySampler(texture.sampler, nullptr);
    device.freeMemory(texture.deviceMemory, nullptr);
  }
}