#include "YTE/Graphics/TextureLoader.hpp"

// Create an image memory barrier for changing the layout of
// an image and put it into an active command buffer

namespace YTE
{
  TextureLoader::TextureLoader(VulkanContext *aContext)
    : mContext(aContext)
  {
    // Create command buffer for submitting image barriers
    // and converting tilings
    vk::CommandBufferAllocateInfo cmdBufInfo = {};
    cmdBufInfo.commandPool = mContext->mCommandPool;
    cmdBufInfo.level = vk::CommandBufferLevel::ePrimary;
    cmdBufInfo.commandBufferCount = 1;

    mCommandBuffer = mContext->mLogicalDevice.allocateCommandBuffers(cmdBufInfo)[0];
  }

  TextureLoader::~TextureLoader()
  {
    mContext->mLogicalDevice.freeCommandBuffers(mContext->mCommandPool, mCommandBuffer);
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

  Texture TextureLoader::loadTexture(const std::string & aFilename)
  {
    auto texture = createTextureImage(aFilename);
    return SetupTexture(texture);
  }

  Texture TextureLoader::loadTexture(std::vector<std::string>& aFilenames)
  {
    auto texture = createTextureImage(aFilenames);
    return SetupTexture(texture);
  }

  void TextureLoader::createImage(u32 aWidth, 
                                  u32 aHeight, 
                                  u32 aImageCount, 
                                  vk::Format aFormat, 
                                  vk::ImageTiling aTiling, 
                                  vk::ImageUsageFlags aUsage, 
                                  vk::MemoryPropertyFlags properties, 
                                  vk::Image& aImage, 
                                  vk::DeviceMemory &aImageMemory)
  {
    //Staging Image creation
    vk::ImageCreateInfo imageInfo;
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.extent.depth = 1;
    imageInfo.extent.width = aWidth;
    
    imageInfo.extent.height = aHeight;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = aImageCount;
    imageInfo.format = aFormat;
    imageInfo.tiling = aTiling;
    
    // We wish to preserve any texels. we would use vk::ImageLayout::eUninitialized
    // if we were using this for a depth or color buffer.
    imageInfo.initialLayout = vk::ImageLayout::ePreinitialized;
    imageInfo.usage = aUsage;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;
    imageInfo.samples = vk::SampleCountFlagBits::e1;
    imageInfo.flags = (vk::ImageCreateFlagBits)0;
    
    aImage = mContext->mLogicalDevice.createImage(imageInfo);

    auto memoryRequirements = mContext->mLogicalDevice.getImageMemoryRequirements(aImage);


    auto deviceMemoryProperties = mContext->mPhysicalDevice.getMemoryProperties();

    vk::MemoryAllocateInfo allocInfo;
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = GetMemoryType(memoryRequirements.memoryTypeBits, deviceMemoryProperties, properties);

    aImageMemory = mContext->mLogicalDevice.allocateMemory(allocInfo);
    mContext->mLogicalDevice.bindImageMemory(aImage, aImageMemory, 0);
  }

  vk::CommandBuffer TextureLoader::beginSingleTimeCommands()
  {
    vk::CommandBufferAllocateInfo allocInfo = {};
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandPool = mContext->mCommandPool;
    allocInfo.commandBufferCount = 1;

    vk::CommandBuffer commandBuffer = mContext->mLogicalDevice.allocateCommandBuffers(allocInfo)[0];

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

    mContext->mQueue.submit(submitInfo, nullptr);
    mContext->mQueue.waitIdle();

    mContext->mLogicalDevice.freeCommandBuffers(mContext->mCommandPool, commandBuffer);
  }
  
  Texture TextureLoader::createTextureImage(std::vector<std::string> &aTextureFiles)
  {
    Texture texture;

    std::vector<STBImageHolder> rawTexture;
    rawTexture.resize(aTextureFiles.size());

    u32 i = 0;

    for (auto &filename : aTextureFiles)
    {
      rawTexture[i].mPixels.reset(stbi_load(filename.c_str(),
                                            &rawTexture[i].mWidth,
                                            &rawTexture[i].mHeight,
                                            &rawTexture[i].mChannels,
                                            STBI_rgb_alpha));

      auto size = rawTexture[0].mWidth * rawTexture[0].mHeight;

      //rawTexture[0].mConvertedPixels.resize(size);
      //
      //for (u32 i = 0; i < size; ++i)
      //{
      //  Pixel<u8> *rawPixels = (Pixel<u8> *)rawTexture[0].mPixels.get();
      //  rawTexture[0].mConvertedPixels[i].mR = rawPixels[i].mR;
      //  rawTexture[0].mConvertedPixels[i].mG = rawPixels[i].mG;
      //  rawTexture[0].mConvertedPixels[i].mB = rawPixels[i].mB;
      //  rawTexture[0].mConvertedPixels[i].mA = rawPixels[i].mA;
      //}

      ++i;
    }

    return createTextureImage(rawTexture);
  }

  Texture TextureLoader::createTextureImage(const std::string &aTextureFile)
  {
    Texture texture;

    std::vector<STBImageHolder> rawTexture;
    rawTexture.resize(1);

    rawTexture[0].mPixels.reset(stbi_load(aTextureFile.c_str(),
                                &rawTexture[0].mWidth,
                                &rawTexture[0].mHeight,
                                &rawTexture[0].mChannels,
                                STBI_rgb_alpha));

    auto size = rawTexture[0].mWidth * rawTexture[0].mHeight;

    //rawTexture[0].mConvertedPixels.resize(size);
    //
    //for (u32 i = 0; i < size; ++i)
    //{
    //  Pixel<u8> *rawPixels = (Pixel<u8> *)rawTexture[0].mPixels.get();
    //  rawTexture[0].mConvertedPixels[i].mR = rawPixels[i].mR;
    //  rawTexture[0].mConvertedPixels[i].mG = rawPixels[i].mG;
    //  rawTexture[0].mConvertedPixels[i].mB = rawPixels[i].mB;
    //  rawTexture[0].mConvertedPixels[i].mA = rawPixels[i].mA;
    //}

    return createTextureImage(rawTexture);
  }




  Texture TextureLoader::createTextureImage(std::vector<STBImageHolder> &aRawTexures)
  {
    assert(aRawTexures.size() > 0 && "No textures sent to be created!");
    Texture texture;


    u32 width = aRawTexures[0].mWidth;
    texture.mHeight = width;

    u32 height = aRawTexures[0].mHeight;
    texture.mHeight = height;

    texture.mLayerCount = static_cast<u32>(aRawTexures.size());

    //vk::DeviceSize totalImageSize = 0;
    vk::DeviceSize singleImageSize = width * height * 4;

    for (auto &holder : aRawTexures)
    {
      //totalImageSize += holder.mWidth * holder.mHeight * 4;

      assert(width == holder.mWidth &&
             height == holder.mHeight &&
             "Textures have mismatched width/height! Currently must be the same");
    }

    //vk::Image stagingImage;
    //vk::DeviceMemory stagingImageMemory;
    //
    //createImage(width,
    //            height,
    //            static_cast<u32>(aRawTexures.size()),
    //            vk::Format::eR8G8B8A8Uint,
    //            vk::ImageTiling::eOptimal,
    //            vk::ImageUsageFlagBits::eTransferSrc,
    //            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
    //            stagingImage,
    //            stagingImageMemory);



    // TODO: Make the size of the Pixels/ImageFormat configurable.
    auto bufferSize = width * height * aRawTexures.size() * sizeof(Pixel<u8>);
    
    auto stagingBuffer = mContext->CreateBuffer(static_cast<u32>(bufferSize),
                                                vk::BufferUsageFlagBits::eTransferSrc,
                                                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    u8 *data = (u8*)mContext->mLogicalDevice.mapMemory(stagingBuffer.mMemory, 0, singleImageSize);

    u32 i = 0;
    for (auto &texture : aRawTexures)
    {
      memcpy(data + (i * singleImageSize), texture.mPixels.get(), (size_t)singleImageSize);
      ++i;
    }

    mContext->mLogicalDevice.unmapMemory(stagingBuffer.mMemory);

    createImage(width,
                height,
                static_cast<u32>(aRawTexures.size()),
                vk::Format::eR8G8B8A8Uint,
                vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
                vk::MemoryPropertyFlagBits::eDeviceLocal,
                texture.mImage,
                texture.mDeviceMemory
                );

    //transitionImageLayout(stagingImage, vk::ImageLayout::ePreinitialized, vk::ImageLayout::eTransferSrcOptimal);
    transitionImageLayout(texture.mImage, vk::ImageLayout::ePreinitialized, vk::ImageLayout::eTransferDstOptimal);
    copyImage(stagingBuffer.mBuffer, texture.mImage, width, height, static_cast<u32>(aRawTexures.size()));
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


  void TextureLoader::copyImage(vk::Buffer srcImageBuffer, vk::Image dstImage, u32 aWidth, u32 aHeight, u32 aLayerCount)
  {
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

    vk::ImageSubresourceLayers subResource = {};
    subResource.aspectMask = vk::ImageAspectFlagBits::eColor;
    subResource.baseArrayLayer = 0;
    subResource.mipLevel = 0;
    subResource.layerCount = aLayerCount;

    vk::BufferImageCopy region = {};
    region.imageSubresource = subResource;
    region.imageExtent.width = aWidth;
    region.imageExtent.height = aHeight;
    region.imageExtent.depth = 1;
    region.bufferOffset = 0;
    region.imageOffset = { 0, 0, 0 };
    region.bufferImageHeight = aHeight;
    region.bufferRowLength = aWidth;

    commandBuffer.copyBufferToImage(srcImageBuffer,
                                    dstImage,
                                    vk::ImageLayout::eTransferDstOptimal,
                                    region);

    endSingleTimeCommands(commandBuffer);
  }

  void TextureLoader::copyImage(vk::Image srcImage, vk::Image dstImage, u32 width, u32 height, u32 aLayerCount)
  {
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

    vk::ImageSubresourceLayers subResource = {};
    subResource.aspectMask = vk::ImageAspectFlagBits::eColor;
    subResource.baseArrayLayer = 0;
    subResource.mipLevel = 0;
    subResource.layerCount = aLayerCount;

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

  void TextureLoader::createImageView(Texture &aTexture, vk::Format aFormat)
  {
    vk::ImageViewCreateInfo viewInfo = {};
    viewInfo.image = aTexture.mImage;
    viewInfo.viewType = (aTexture.mLayerCount > 1) ? vk::ImageViewType::e2DArray : vk::ImageViewType::e2D;
    viewInfo.format = aFormat;
    viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = aTexture.mLayerCount;

    aTexture.mView = mContext->mLogicalDevice.createImageView(viewInfo);
  }

  void TextureLoader::createTextureImageView(Texture &aTexture)
  {
    createImageView(aTexture, vk::Format::eR8G8B8A8Uint);
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

    aTexture.mSampler = mContext->mLogicalDevice.createSampler(samplerInfo);
  }

  // Load a 2D texture
  Texture TextureLoader::SetupTexture(Texture &aTexture)
  {
    createTextureSampler(aTexture);

    createTextureImageView(aTexture);

    // Fill descriptor image info that can be used for setting up descriptor sets
    aTexture.mDescriptor.imageLayout = vk::ImageLayout::eGeneral;
    aTexture.mDescriptor.imageView = aTexture.mView;
    aTexture.mDescriptor.sampler = aTexture.mSampler;

    return aTexture;
  }

  // Clean up vulkan resources used by a texture object
  void TextureLoader::destroyTexture(Texture texture)
  {
    mContext->mLogicalDevice.destroyImageView(texture.mView, nullptr);
    mContext->mLogicalDevice.destroyImage(texture.mImage, nullptr);
    mContext->mLogicalDevice.destroySampler(texture.mSampler, nullptr);
    mContext->mLogicalDevice.freeMemory(texture.mDeviceMemory, nullptr);
  }

  inline vk::CommandBuffer TextureLoader::createCommandBuffer(vk::CommandBufferLevel level, bool begin)
  {
    vk::CommandBufferAllocateInfo cmdBufAllocateInfo;
    cmdBufAllocateInfo.commandPool = mContext->mCommandPool;
    cmdBufAllocateInfo.level = level;
    cmdBufAllocateInfo.commandBufferCount = 1;

    auto cmdBuffer = mContext->mLogicalDevice.allocateCommandBuffers(cmdBufAllocateInfo)[0];

    // If requested, also start the new command buffer
    if (begin)
    {
      vk::CommandBufferBeginInfo cmdBufInfo;
      cmdBuffer.begin(cmdBufInfo);
    }

    return cmdBuffer;
  }
}