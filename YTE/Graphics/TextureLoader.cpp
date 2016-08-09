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
    subresourceRange.baseMipLevel = 1;
    subresourceRange.levelCount = 1;
    subresourceRange.layerCount = 1;
    setImageLayout(cmdbuffer, image, aspectMask, oldImageLayout, newImageLayout, subresourceRange);
  }

  void TextureLoader::createImage(uint32_t aWidth, uint32_t aHeight, vk::Format aFormat, vk::ImageTiling aTiling, vk::ImageUsageFlags aUsage, vk::MemoryPropertyFlags properties, vk::Image& aImage, vk::DeviceMemory &aImageMemory)
  {
    //Staging Image creation
    vk::ImageCreateInfo imageStagingInfo;
    imageStagingInfo.imageType = vk::ImageType::e2D;
    imageStagingInfo.extent.depth = 1;
    imageStagingInfo.extent.width = aWidth;
    imageStagingInfo.extent.height = aHeight;
    imageStagingInfo.mipLevels = 1;
    imageStagingInfo.arrayLayers = 1;
    imageStagingInfo.format = aFormat;
    imageStagingInfo.tiling = aTiling;

    // We wish to preserve any texels. we would use vk::ImageLayout::eUninitialized
    // if we were using this for a depth or color buffer.
    imageStagingInfo.initialLayout = vk::ImageLayout::ePreinitialized;
    imageStagingInfo.usage = aUsage;
    imageStagingInfo.sharingMode = vk::SharingMode::eExclusive;
    imageStagingInfo.samples = vk::SampleCountFlagBits::e1;
    imageStagingInfo.flags = (vk::ImageCreateFlagBits)0;

    aImage = device.createImage(imageStagingInfo);

    auto memoryRequirements = device.getImageMemoryRequirements(aImage);

    vk::MemoryAllocateInfo allocInfo;
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = GetMemoryType(memoryRequirements.memoryTypeBits, deviceMemoryProperties, properties);

    aImageMemory = device.allocateMemory(allocInfo);
    device.bindImageMemory(aImage, aImageMemory, 0);
  }

  vk::CommandBuffer TextureLoader::beginSingleTimeCommands()
  {
    vk::CommandBufferAllocateInfo allocInfo = {};
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandPool = cmdPool;
    allocInfo.commandBufferCount = 1;

    vk::CommandBuffer commandBuffer = device.allocateCommandBuffers(allocInfo)[0];

    vk::CommandBufferBeginInfo beginInfo = {};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    commandBuffer.begin(beginInfo);

    return commandBuffer;
  }

  void TextureLoader::endSingleTimeCommands(vk::CommandBuffer commandBuffer)
  {
    commandBuffer.end();

    vk::SubmitInfo submitInfo = {};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    queue.submit(submitInfo, nullptr);
    queue.waitIdle();

    device.freeCommandBuffers(cmdPool, commandBuffer);
  }

  Texture TextureLoader::createTextureImage(const std::string &aTextureFile)
  {
    Texture texture;

    int texWidth, texHeight, texChannels;

    stbi_uc* pixels = stbi_load(aTextureFile.c_str(), 
                                &texWidth, 
                                &texHeight, 
                                &texChannels, 
                                STBI_rgb_alpha);
    vk::DeviceSize imageSize = texWidth * texHeight * 4;

    texture.mWidth = static_cast<u32>(texWidth);
    texture.mHeight = static_cast<u32>(texHeight);

    assert(pixels && "failed to load texture image!");

    vk::Image stagingImage;
    vk::DeviceMemory stagingImageMemory;

    createImage(texWidth, 
                texHeight, 
                vk::Format::eR8G8B8A8Unorm, 
                vk::ImageTiling::eLinear, 
                vk::ImageUsageFlagBits::eTransferSrc, 
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, 
                stagingImage, 
                stagingImageMemory);

    void* data = device.mapMemory(stagingImageMemory, 0, imageSize);
    memcpy(data, pixels, (size_t)imageSize);
    device.unmapMemory(stagingImageMemory);

    stbi_image_free(pixels);

    createImage(texWidth,
                texHeight,
                vk::Format::eR8G8B8A8Unorm,
                vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
                vk::MemoryPropertyFlagBits::eDeviceLocal,
                texture.mImage,
                texture.mDeviceMemory
                );

    transitionImageLayout(stagingImage, vk::ImageLayout::ePreinitialized, vk::ImageLayout::eTransferSrcOptimal);
    transitionImageLayout(texture.mImage, vk::ImageLayout::ePreinitialized, vk::ImageLayout::eTransferDstOptimal);
    copyImage(stagingImage, texture.mImage, texWidth, texHeight);
    transitionImageLayout(texture.mImage, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

    return texture;
  }

  void TextureLoader::transitionImageLayout(vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) 
  {
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

    vk::ImageMemoryBarrier barrier = {};
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    if (oldLayout == vk::ImageLayout::ePreinitialized && newLayout == vk::ImageLayout::eTransferSrcOptimal)
    {
      barrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
    }
    else if (oldLayout == vk::ImageLayout::ePreinitialized && newLayout == vk::ImageLayout::eTransferDstOptimal)
    {
      barrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
    }
    else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
      barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
    }
    else 
    {
      assert(false && "unsupported layout transition!");
    }

    commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                                  vk::PipelineStageFlagBits::eTopOfPipe, 
                                  vk::DependencyFlags(), 
                                  nullptr, 
                                  nullptr, 
                                  barrier);

    endSingleTimeCommands(commandBuffer);
  }

  void TextureLoader::copyImage(vk::Image srcImage, vk::Image dstImage, u32 width, u32 height)
  {
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

    vk::ImageSubresourceLayers subResource = {};
    subResource.aspectMask = vk::ImageAspectFlagBits::eColor;
    subResource.baseArrayLayer = 0;
    subResource.mipLevel = 0;
    subResource.layerCount = 1;

    vk::ImageCopy region = {};
    region.srcSubresource = subResource;
    region.dstSubresource = subResource;
    region.srcOffset = { 0, 0, 0 };
    region.dstOffset = { 0, 0, 0 };
    region.extent.width = width;
    region.extent.height = height;
    region.extent.depth = 1;

    commandBuffer.copyImage(srcImage, 
                            vk::ImageLayout::eTransferSrcOptimal,
                            dstImage, 
                            vk::ImageLayout::eTransferDstOptimal,
                            region);

    endSingleTimeCommands(commandBuffer);
  }

  void TextureLoader::createImageView(vk::Image aImage, vk::Format aFormat, vk::ImageView &aImageView)
  {
    vk::ImageViewCreateInfo viewInfo = {};
    viewInfo.image = aImage;
    viewInfo.viewType = vk::ImageViewType::e2D;
    viewInfo.format = aFormat;
    viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    aImageView = device.createImageView(viewInfo);
  }

  void TextureLoader::createTextureImageView(Texture &aTexture)
  {
    createImageView(aTexture.mImage, vk::Format::eR8G8B8A8Unorm, aTexture.mView);
  }


  // TODO: Make everything in this function configurable.
  void TextureLoader::createTextureSampler(Texture &aTexture)
  {
    vk::SamplerCreateInfo samplerInfo = {};

    samplerInfo.magFilter = vk::Filter::eLinear;
    samplerInfo.minFilter = vk::Filter::eLinear;

    samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
    samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
    samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;

    samplerInfo.anisotropyEnable = true;
    samplerInfo.maxAnisotropy = 16;

    samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
    samplerInfo.unnormalizedCoordinates = false;

    samplerInfo.compareEnable = true;
    samplerInfo.compareOp = vk::CompareOp::eAlways;

    samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    aTexture.mSampler = device.createSampler(samplerInfo);
  }

  // Load a 2D texture

  Texture TextureLoader::loadTexture(const std::string &filename)
  {
    auto texture = createTextureImage(filename);

    createTextureSampler(texture);

    createTextureImageView(texture);

    // Fill descriptor image info that can be used for setting up descriptor sets
    texture.mDescriptor.imageLayout = vk::ImageLayout::eGeneral;
    texture.mDescriptor.imageView = texture.mView;
    texture.mDescriptor.sampler = texture.mSampler;

    return texture;
  }

  // Clean up vulkan resources used by a texture object

  void TextureLoader::destroyTexture(Texture texture)
  {
    device.destroyImageView(texture.mView, nullptr);
    device.destroyImage(texture.mImage, nullptr);
    device.destroySampler(texture.mSampler, nullptr);
    device.freeMemory(texture.mDeviceMemory, nullptr);
  }
  inline vk::CommandBuffer TextureLoader::createCommandBuffer(vk::CommandBufferLevel level, bool begin)
  {
    vk::CommandBufferAllocateInfo cmdBufAllocateInfo;
    cmdBufAllocateInfo.commandPool = cmdPool;
    cmdBufAllocateInfo.level = level;
    cmdBufAllocateInfo.commandBufferCount = 1;

    auto cmdBuffer = device.allocateCommandBuffers(cmdBufAllocateInfo)[0];

    // If requested, also start the new command buffer
    if (begin)
    {
      vk::CommandBufferBeginInfo cmdBufInfo;
      cmdBuffer.begin(cmdBufInfo);
    }

    return cmdBuffer;
  }
}