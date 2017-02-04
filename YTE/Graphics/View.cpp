#include "YTE/Graphics/View.hpp"
#include "YTE/Graphics/VulkanRenderer.hpp"

namespace YTE
{
  View::View(VulkanRenderer * aContext)
    : mContext(aContext)
  {

  }

  View::~View()
  {
    mUniform.Destruct();
  }

  void View::Initialize()
  {
    mUniform.mLogicalDevice = &mContext->mLogicalDevice;

    // Prepare and initialize a uniform buffer block containing shader uniforms
    // In Vulkan there are no more single uniforms like in GL
    // All shader uniforms are passed as uniform buffer blocks 

    // Vertex shader uniform buffer block

    // Most implementations offer multiple memory types and selecting the 
    // correct one to allocate memory from is important
    // We also want the buffer to be host coherent so we don't have to flush 
    // after every update. 
    // Note that this may affect performance so you might not want to do this 
    // in a real world application that updates buffers on a regular base
    mUniform = mContext->CreateBuffer(sizeof(UniformBufferObject),
                                      vk::BufferUsageFlagBits::eUniformBuffer,
                                      vk::MemoryPropertyFlagBits::eHostVisible |
                                      vk::MemoryPropertyFlagBits::eHostCoherent);

    // Store information in the uniform's descriptor
    mUniformBufferInfo.buffer = mUniform.mBuffer;
    mUniformBufferInfo.offset = 0;
    mUniformBufferInfo.range = sizeof(UniformBufferObject);

    UpdateUniformBuffers(true);
  }

  inline void View::UpdateUniformBuffers(bool aRecalculation)
  {
    if (aRecalculation)
    {
      // Update matrices
      mUniformBufferData.mProjectionMatrix = glm::perspective(glm::radians(60.0f), (float)mWidth / (float)mHeight, 0.1f, 256.0f);

      auto viewMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, mCameraPosition.z));

      mUniformBufferData.mModelMatrix = viewMatrix * glm::translate(glm::mat4(), mCameraPosition);
      mUniformBufferData.mModelMatrix = Rotate(mUniformBufferData.mModelMatrix, mRotation);

      //mUniformBufferData.mModelMatrix = glm::rotate(mUniformBufferData.mModelMatrix, 
      //                                              glm::radians(mRotation.x), 
      //                                              glm::vec3(1.0f, 0.0f, 0.0f));
      //mUniformBufferData.mModelMatrix = glm::rotate(mUniformBufferData.mModelMatrix, 
      //                                              glm::radians(mRotation.y), 
      //                                              glm::vec3(0.0f, 1.0f, 0.0f));
      
      //mUniformBufferData.mModelMatrix = glm::rotate(mUniformBufferData.mModelMatrix, 
      //                                              glm::radians(mRotation.z), 
      //                                              glm::vec3(0.0f, 0.0f, 1.0f));

      mUniformBufferData.mViewPosition = mCameraPosition;
    }

    // Map uniform buffer and update it
    u8 *data = (u8*)mContext->mLogicalDevice.mapMemory(mUniform.mMemory,
                                                       0,
                                                       sizeof(UniformBufferObject));
    memcpy(data, &mUniformBufferData, sizeof(UniformBufferObject));
    mContext->mLogicalDevice.unmapMemory(mUniform.mMemory);
  }

}