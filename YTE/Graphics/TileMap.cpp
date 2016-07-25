//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#include "YTE/Graphics/TileMap.hpp"
#include "TileMap.hpp"

namespace YTE
{
  TileMap::TileMap(std::vector<Tile> aMap, std::vector<std::string> aTextureFiles, VulkanContext *aContext)
    : mMap(aMap), mContext(aContext)
  {
    for (auto &tile : aMap)
    {
      assert(tile.mTile <= aTextureFiles.size() && "Malformed tile input.");
    }

    TextureLoader loader(mContext->mPhysicalDevice, mContext->mLogicalDevice, mContext->mQueue, mContext->mCommandPool);

    mTextures.reserve(aTextureFiles.size());

    for (auto &textureName : aTextureFiles)
    {
      mTextures.emplace_back(loader.loadTexture(textureName.c_str()));
    }


















    // Create our Index buffer
    vk::MemoryAllocateInfo memAlloc;

    // Index buffer
    vk::BufferCreateInfo indexbufferInfo = {};
    indexbufferInfo.size = 6 * sizeof(u32);
    indexbufferInfo.usage = vk::BufferUsageFlagBits::eIndexBuffer;

    mContext->mLogicalDevice.createBuffer(&indexbufferInfo, nullptr, &mIndexMemory.mBuffer);
    auto requirements = mContext->mLogicalDevice.getBufferMemoryRequirements(mIndexMemory.mBuffer);

    memAlloc.allocationSize = requirements.size;


    memAlloc.memoryTypeIndex = GetMemoryType(requirements.memoryTypeBits, mContext->mPhysicalMemoryProperties,vk::MemoryPropertyFlagBits::eHostVisible);
    mIndexMemory.mMemory = mContext->mLogicalDevice.allocateMemory(memAlloc);

    void *indexPtr = mContext->mLogicalDevice.mapMemory(mIndexMemory.mMemory, 0, indexbufferInfo.size);
    auto indices = (u32*)indexPtr;
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 2;
    indices[4] = 3;
    indices[5] = 0;

    mContext->mLogicalDevice.unmapMemory(mIndexMemory.mMemory);

    mContext->mLogicalDevice.bindBufferMemory(mIndexMemory.mBuffer, mIndexMemory.mMemory, 0);

    // Create our vertex buffer:
    auto vertexInputBufferInfo = vk::BufferCreateInfo()
                                      .setSize(sizeof(Triangle) * 4) // Size in bytes.
                                      .setUsage(vk::BufferUsageFlagBits::eVertexBuffer)
                                      .setSharingMode(vk::SharingMode::eExclusive); // TODO: Change to not exclusive.

    mVertexMemory.mBuffer = mContext->mLogicalDevice.createBuffer(vertexInputBufferInfo);

    vulkan_assert(mVertexMemory.mBuffer, "Failed to create vertex input buffer.");

    auto vertexBufferMemoryRequirements = mContext->mLogicalDevice.getBufferMemoryRequirements(mVertexMemory.mBuffer);

    auto bufferAllocateInfo = vk::MemoryAllocateInfo()
                                   .setAllocationSize(vertexBufferMemoryRequirements.size);

    u32 vertexMemoryTypeBits = vertexBufferMemoryRequirements.memoryTypeBits;
    vk::MemoryPropertyFlags vertexDesiredMemoryFlags = vk::MemoryPropertyFlagBits::eHostVisible;

    u32 i = 0;
    for (auto &memoryType : mContext->mPhysicalMemoryProperties.memoryTypes)
    {
      if (vertexMemoryTypeBits & 1) 
      {
        if ((memoryType.propertyFlags & vertexDesiredMemoryFlags) == vertexDesiredMemoryFlags) 
        {
          bufferAllocateInfo.memoryTypeIndex = (u32)i;
          break;
        }
      }

      vertexMemoryTypeBits = vertexMemoryTypeBits >> 1;

      ++i;
    }

    auto vertexBufferMemory = mContext->mLogicalDevice.allocateMemory(bufferAllocateInfo);
    vulkan_assert(vertexBufferMemory, "Failed to allocate buffer memory.");

    void *mapped = mContext->mLogicalDevice.mapMemory(vertexBufferMemory, 0, VK_WHOLE_SIZE);
    vulkan_assert(mapped, "Failed to map buffer memory.");

    auto mQuad = (Quad*)mapped;
    mQuad->mVertex1.mPosition = { -0.5f, -0.5f, 0.0f, 1.0f };
    mQuad->mVertex1.mUVCoordinates = { 0.0f, 0.0f};
    mQuad->mVertex1.mNormal = { 0.0f, 0.0f, 1.0};

    mQuad->mVertex2.mPosition = { 0.5f, -0.5f, 0.0f, 1.0f };
    mQuad->mVertex2.mUVCoordinates = { 1.0f, 0.0f };
    mQuad->mVertex2.mNormal = { 0.0f, 0.0f, 1.0 };

    mQuad->mVertex3.mPosition = { 0.5f, 0.5f, 0.0f, 1.0f };
    mQuad->mVertex3.mUVCoordinates = { 1.0f, 1.0f };
    mQuad->mVertex3.mNormal = { 0.0f, 0.0f, 1.0 };

    mQuad->mVertex4.mPosition = { -0.5f, 0.5f, 0.0f, 1.0f };
    mQuad->mVertex4.mUVCoordinates = { 0.0f, 1.0f };
    mQuad->mVertex4.mNormal = { 0.0f, 0.0f, 1.0 };

    mContext->mLogicalDevice.unmapMemory(vertexBufferMemory);

    mContext->mLogicalDevice.bindBufferMemory(mVertexMemory.mBuffer, vertexBufferMemory, 0);
  }

  TileMap::~TileMap()
  {
  }

  void TileMap::Draw(u32 aImageId)
  {
    vk::CommandBufferBeginInfo beginInfo = {};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    vk::Viewport viewport{ 0, 0, static_cast<float>(mContext->mWidth), static_cast<float>(mContext->mHeight), 0, 1 };
    vk::Rect2D scissor{ { 0, 0 },{ mContext->mWidth, mContext->mHeight } };


    vk::ClearValue clearValue[2];
    clearValue[0].color.float32[0] = 0.0f;
    clearValue[0].color.float32[1] = 0.7f;
    clearValue[0].color.float32[2] = 1.0f;
    clearValue[0].color.float32[3] = 0.0f;
    clearValue[1].color.float32[0] = 0.0f;
    clearValue[1].color.float32[2] = 0.0f;

    vk::RenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.renderPass = mContext->mRenderPass;
    renderPassBeginInfo.framebuffer = mContext->mFrameBuffers[aImageId];
    renderPassBeginInfo.renderArea = { 0, 0, mContext->mWidth, mContext->mHeight };
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValue;

    //for (auto &tile : mMap)
    for (auto tileIt = mMap.rbegin(); tileIt < mMap.rend(); ++tileIt)
    {
      auto &tile = *tileIt;
      mContext->UpdateDescriptorSet(mTextures[tile.mTile]);

      mContext->UpdateUniformBuffers(&tile);

      // change image layout from VK_IMAGE_LAYOUT_PRESENT_SRC_KHR to VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
      vk::ImageMemoryBarrier layoutTransitionBarrier = {};
      layoutTransitionBarrier.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
      layoutTransitionBarrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
                                              vk::AccessFlagBits::eColorAttachmentWrite;
      layoutTransitionBarrier.oldLayout = vk::ImageLayout::ePresentSrcKHR;
      layoutTransitionBarrier.newLayout = vk::ImageLayout::eColorAttachmentOptimal;
      layoutTransitionBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      layoutTransitionBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      layoutTransitionBarrier.image = mContext->mPresentImages[aImageId];

      auto resourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
      layoutTransitionBarrier.subresourceRange = resourceRange;


      auto result = mContext->mDrawCommandBuffer.begin(&beginInfo);

      mContext->mDrawCommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                                                   vk::PipelineStageFlagBits::eTopOfPipe,
                                                   vk::DependencyFlags(),
                                                   nullptr,
                                                   nullptr,
                                                   layoutTransitionBarrier);

      mContext->mDrawCommandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);


      // bind the graphics pipeline to the command buffer. Any vkDraw command afterwards is affected by this pipeline!
      mContext->mDrawCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mContext->mPipeline);

      // Bind descriptor sets describing shader binding points
      mContext->mDrawCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mContext->mPipelineLayout, 0, mContext->mDescriptorSets, nullptr);

      // take care of dynamic state:
      mContext->mDrawCommandBuffer.setViewport(0, viewport);
      mContext->mDrawCommandBuffer.setScissor(0, scissor);

      mContext->mDrawCommandBuffer.bindVertexBuffers(0, mVertexMemory.mBuffer, vk::DeviceSize());
      mContext->mDrawCommandBuffer.bindIndexBuffer(mIndexMemory.mBuffer, 0, vk::IndexType::eUint32);

      
      
      
      
      
      mContext->mDrawCommandBuffer.drawIndexed(6, 1, 0, 0, 1);

      mContext->mDrawCommandBuffer.endRenderPass();


      
      
      
      
      
      
      
      
      
      
      
      
      // change layout back to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
      vk::ImageMemoryBarrier prePresentBarrier = {};
      prePresentBarrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
      prePresentBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
      prePresentBarrier.oldLayout = vk::ImageLayout::eColorAttachmentOptimal;
      prePresentBarrier.newLayout = vk::ImageLayout::ePresentSrcKHR;
      prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      prePresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
      prePresentBarrier.image = mContext->mPresentImages[aImageId];

      mContext->mDrawCommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands,
                                                   vk::PipelineStageFlagBits::eBottomOfPipe,
                                                   vk::DependencyFlags(),
                                                   nullptr,
                                                   nullptr,
                                                   prePresentBarrier);
      mContext->mDrawCommandBuffer.end();
    }
  }
};

