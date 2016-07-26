//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#include "YTE/Graphics/TextureLoader.hpp"
#include "YTE/Graphics/TileMap.hpp"

#include "gtc/matrix_transform.hpp"

namespace YTE
{
  TileMap::TileMap(std::vector<Tile> &aMap, std::vector<std::string> &aTextureFiles, VulkanContext *aContext)
    : mMap(aMap), mContext(aContext)
  {
    for (auto &tile : aMap)
    {
      assert(tile.mTile <= aTextureFiles.size() && "Malformed tile input.");
      //tile.mPosition.y *= -1;
    }

    TextureLoader loader(mContext);

    //for (auto &textureFile : aTextureFiles)
    //{
    //  mTextures.emplace_back(loader.loadTexture(textureFile));
    //}


    mTexture = loader.loadTexture(aTextureFiles);

















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

    PrepareInstanceData();
  }

  TileMap::~TileMap()
  {
  }

  void TileMap::PrepareInstanceData()
  {
    std::vector<InstanceData> instanceData;
    instanceData.resize(mMap.size());
    u32 i = 0;

    auto modelMatrix = glm::translate(glm::mat4(), mPosition);

    for (auto &tile : mMap)
    {
      instanceData[i].mPostion = tile.mPosition;
      instanceData[i].mTile = tile.mTile;
      ++i;
    }

    mInstanceBuffer.mSize = instanceData.size() * sizeof(InstanceData);

    // Staging
    // Instanced data is static, copy to device local memory 
    // This results in better performance
    auto stagingMemory = mContext->CreateBuffer(mInstanceBuffer.mSize,
                                                vk::BufferUsageFlagBits::eTransferSrc,
                                                vk::MemoryPropertyFlagBits::eHostVisible);

    u8 *data = (u8*)mContext->mLogicalDevice.mapMemory(stagingMemory.mMemory, 0, mInstanceBuffer.mSize);
    memcpy(data, instanceData.data(), mInstanceBuffer.mSize);
    mContext->mLogicalDevice.unmapMemory(stagingMemory.mMemory);

    mInstanceBuffer.mBufferMemory =  mContext->CreateBuffer(mInstanceBuffer.mSize,
                                                            vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                                                            vk::MemoryPropertyFlagBits::eDeviceLocal);

    // Copy to staging buffer
    mContext->CopyBuffer(stagingMemory.mBuffer, mInstanceBuffer.mBufferMemory.mBuffer, mInstanceBuffer.mSize);

    mInstanceBuffer.mDescriptor.range = mInstanceBuffer.mSize;
    mInstanceBuffer.mDescriptor.buffer = mInstanceBuffer.mBufferMemory.mBuffer;
    mInstanceBuffer.mDescriptor.offset = 0;

    // Destroy staging resources
    mContext->mLogicalDevice.destroyBuffer(stagingMemory.mBuffer);
    mContext->mLogicalDevice.freeMemory(stagingMemory.mMemory);
  }

  void TileMap::Draw()
  {
  
  }


  void TileMap::SetupCommandBuffer()
  {
    auto &commandBuffer = mContext->mDrawCommandBuffers[mContext->mCurrentDrawBuffer];

    vk::CommandBufferBeginInfo beginInfo = {};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    vk::Viewport viewport{ 0, 0, static_cast<float>(mContext->mWidth), static_cast<float>(mContext->mHeight), 0, 1 };
    vk::Rect2D scissor{ { 0, 0 },{ mContext->mWidth, mContext->mHeight } };

    std::array<vk::ClearValue, 2> clearValues;
    //clearValues[0].color.float32[0] = 0.0f;
    //clearValues[0].color.float32[1] = 0.0f;
    //clearValues[0].color.float32[2] = 0.0f;
    //clearValues[0].color.float32[3] = 0.0f;
    //clearValues[1].color.float32[0] = 0.0f;
    //clearValues[1].color.float32[2] = 0.0f;

    clearValues[0].color.float32[0] = 0.0f;
    clearValues[0].color.float32[1] = 0.0f;
    clearValues[0].color.float32[2] = 0.0f;
    clearValues[0].color.float32[3] = 0.0f;
    clearValues[1].color.float32[0] = 0.0f;
    clearValues[1].color.float32[2] = 0.0f;

    vk::RenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.renderPass = mContext->mRenderPass;
    renderPassBeginInfo.framebuffer = mContext->mFrameBuffers[mContext->mCurrentDrawBuffer];
    renderPassBeginInfo.renderArea = { 0, 0, mContext->mWidth, mContext->mHeight };
    renderPassBeginInfo.clearValueCount = static_cast<u32>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    // change image layout from VK_IMAGE_LAYOUT_PRESENT_SRC_KHR to VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    vk::ImageMemoryBarrier layoutTransitionBarrier = {};
    layoutTransitionBarrier.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
    layoutTransitionBarrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
                                            vk::AccessFlagBits::eColorAttachmentWrite;
    layoutTransitionBarrier.oldLayout = vk::ImageLayout::ePresentSrcKHR;
    layoutTransitionBarrier.newLayout = vk::ImageLayout::eColorAttachmentOptimal;
    layoutTransitionBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    layoutTransitionBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    layoutTransitionBarrier.image = mContext->mPresentImages[mContext->mCurrentDrawBuffer];

    auto resourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
    layoutTransitionBarrier.subresourceRange = resourceRange;

    auto result = commandBuffer.begin(&beginInfo);

    commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                                  vk::PipelineStageFlagBits::eTopOfPipe,
                                  vk::DependencyFlags(),
                                  nullptr,
                                  nullptr,
                                  layoutTransitionBarrier);

    commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);


    // bind the graphics pipeline to the command buffer. Any vkDraw command afterwards is affected by this pipeline!
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mContext->mPipeline);

    // Bind descriptor sets describing shader binding points
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mContext->mPipelineLayout, 0, mContext->mDescriptorSets, nullptr);

    // take care of dynamic state:
    commandBuffer.setViewport(0, viewport);
    commandBuffer.setScissor(0, scissor);

    // Binding point 0 : Mesh vertex buffer
    commandBuffer.bindVertexBuffers(0, mVertexMemory.mBuffer, vk::DeviceSize());
    // Binding point 1 : Instance data buffer
    commandBuffer.bindVertexBuffers(1, mInstanceBuffer.mBufferMemory.mBuffer, vk::DeviceSize());
    commandBuffer.bindIndexBuffer(mIndexMemory.mBuffer, 0, vk::IndexType::eUint32);

    commandBuffer.drawIndexed(6, static_cast<u32>(mMap.size()), 0, 0, 1);

    commandBuffer.endRenderPass();

    // change layout back to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    vk::ImageMemoryBarrier prePresentBarrier = {};
    prePresentBarrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    prePresentBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
    prePresentBarrier.oldLayout = vk::ImageLayout::eColorAttachmentOptimal;
    prePresentBarrier.newLayout = vk::ImageLayout::ePresentSrcKHR;
    prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    prePresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    prePresentBarrier.image = mContext->mPresentImages[mContext->mCurrentDrawBuffer];

    commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands,
                                  vk::PipelineStageFlagBits::eBottomOfPipe,
                                  vk::DependencyFlags(),
                                  nullptr,
                                  nullptr,
                                  prePresentBarrier);

    commandBuffer.end();
  }
};

