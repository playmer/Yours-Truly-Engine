#pragma once

#include "YTE/Core/Types.hpp"

#include "YTE/Graphics/Texture.hpp"

#include "vulkan/vkel.h"
#include "vulkan/vk_cpp.hpp"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

namespace YTE
{
  inline void vulkan_assert(u64 aFlag, const char *aMessage = "")
  {
    bool check = aFlag ? true : false;
    runtime_assert(check, aMessage);
  }

  inline void vulkan_assert(void *aFlag, const char *aMessage = "")
  {
    bool check = aFlag ? false : true;
    runtime_assert(check, aMessage);
  }

  template<typename Type>
  inline void vulkan_assert(Type aFlag, const char *aMessage = "")
  {
    bool check = static_cast<bool>(aFlag);
    runtime_assert(check, aMessage);
  }

  inline void checkVulkanResult(vk::Result &aResult, const char *aMessage)
  {
    vulkan_assert(aResult == vk::Result::eSuccess, aMessage);
  }


  inline void checkVulkanResult(VkResult &aResult, const char *aMessage)
  {
    vulkan_assert(aResult == VK_SUCCESS, aMessage);
  }

  struct Vertex
  {
    Vertex()
    {
      mPosition = { 0,0,0,0 };
      mUVCoordinates = { 0,0 };
      mNormal = { 0,0,0 };
    }

    void Translate(const glm::vec3 &aTranslation)
    {
      auto translate = glm::translate(glm::mat4(), aTranslation);

      mPosition = translate * mPosition;
    }

    void Scale(const glm::vec3 &aScale)
    {
      auto scale = glm::scale(glm::mat4(), aScale);

      mPosition = scale * mPosition;
    }

    void Rotate(const glm::vec3 &aRotation)
    {
      auto rotation = glm::mat4();
      rotation = glm::rotate(rotation, aRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
      rotation = glm::rotate(rotation, aRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
      rotation = glm::rotate(rotation, aRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

      mPosition = rotation * mPosition;
    }

    glm::vec4 mPosition;
    glm::vec2 mUVCoordinates;
    glm::vec3 mNormal;
  };

  struct Triangle
  {
    void Translate(const glm::vec3 &aTranslation)
    {
      mVertex1.Translate(aTranslation);
      mVertex2.Translate(aTranslation);
      mVertex3.Translate(aTranslation);
    }

    void Scale(const glm::vec3 &aScale)
    {
      mVertex1.Scale(aScale);
      mVertex2.Scale(aScale);
      mVertex3.Scale(aScale);
    }
    void Rotate(const glm::vec3 &aRotation)
    {
      mVertex1.Rotate(aRotation);
      mVertex2.Rotate(aRotation);
      mVertex3.Rotate(aRotation);
    }

    Vertex mVertex1;
    Vertex mVertex2;
    Vertex mVertex3;
  };

  struct Quad
  {
    void Translate(const glm::vec3 &aTranslation)
    {
      mVertex1.Translate(aTranslation);
      mVertex2.Translate(aTranslation);
      mVertex3.Translate(aTranslation);
      mVertex4.Translate(aTranslation);
    }

    void Scale(const glm::vec3 &aScale)
    {
      mVertex1.Scale(aScale);
      mVertex2.Scale(aScale);
      mVertex3.Scale(aScale);
      mVertex4.Scale(aScale);
    }
    void Rotate(const glm::vec3 &aRotation)
    {
      mVertex1.Rotate(aRotation);
      mVertex2.Rotate(aRotation);
      mVertex3.Rotate(aRotation);
      mVertex4.Rotate(aRotation);
    }


    Vertex mVertex1;
    Vertex mVertex2;
    Vertex mVertex3;
    Vertex mVertex4;
  };

  struct UniformBufferObject
  {
    glm::mat4 mProjectionMatrix;
    glm::mat4 mModelMatrix;
    glm::vec4 mViewPosition;
    float mLevelOfDetailBias = 1.0f;
  };

  struct BufferMemory
  {
    vk::DeviceMemory mMemory;
    vk::Buffer mBuffer;
  };

  class VulkanContext
  {
    public:


    template<typename FlagType>
    u32 GetMemoryType(u32 aTypeBits, FlagType aProperties)
    {
      for (uint32_t i = 0; i < 32; i++)
      {
        if ((aTypeBits & 1) == 1)
        {
          if ((mPhysicalMemoryProperties.memoryTypes[i].propertyFlags & aProperties) == aProperties)
          {
            return i;
          }
        }
        aTypeBits >>= 1;
      }

      // todo : throw error
      return 0;
    }

    BufferMemory CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
    void CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

    template <typename Type>
    BufferMemory CreateFilledBuffer(const Type *aData, u64 aSize, bool aUseStaging = true)
    {
      vk::DeviceSize bufferSize = static_cast<u32>(sizeof(Type) * aSize);

      BufferMemory toReturn;

      auto bufferMemory = CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

      void *data = mLogicalDevice.mapMemory(bufferMemory.mMemory, 0, bufferSize);
      memcpy(data, aData, (size_t)bufferSize);
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

    template <typename Type>
    BufferMemory CreateFilledBuffer(std::initializer_list<Type> aData, bool aUseStaging = true)
    {
      return CreateFilledBuffer(aData.begin(), aData.size(), aUseStaging);
    }

    template <typename Type>
    BufferMemory CreateFilledBuffer(std::vector<Type> aData, bool aUseStaging = true)
    {
      return CreateFilledBuffer(aData.data(), aData.size(), aUseStaging);
    }

    template <typename Type, u32 Size>
    BufferMemory CreateFilledBuffer(std::array<Type, Size> aData, bool aUseStaging = true)
    {
      return CreateFilledBuffer(aData.data(), aData.size(), aUseStaging);
    }

    void SetupDescriptorSetLayout();
    void SetupDescriptorPool();
    void SetupDescriptorSet();

    void UpdateDescriptorSet(Texture &aTexture);
    void UpdateUniformBuffers(bool aRecalculation);


    u32 mWidth = 0;
    u32 mHeight = 0;
    float mZoom = -2.5f;

    UniformBufferObject mUniformBufferData;

    glm::vec3 mRotation = glm::vec3();
    glm::vec3 mCameraPosition = glm::vec3();

    vk::Instance mInstance;
    vk::SurfaceKHR mSurface;
    vk::DebugReportCallbackEXT mCallback;
    vk::PhysicalDevice mPhysicalDevice;
    vk::PhysicalDeviceProperties mPhysicalDeviceProperties;
    vk::PhysicalDeviceMemoryProperties mPhysicalMemoryProperties;

    vk::CommandPool mCommandPool;

    i32 mPresentQueueIdx;
    vk::Device mLogicalDevice;
    vk::SwapchainKHR mSwapChain;

    vk::Queue mQueue;
    vk::CommandBuffer mSetupCommandBuffer;

    std::vector<vk::CommandBuffer> mDrawCommandBuffers;
    u32 mCurrentDrawBuffer;

    std::vector<vk::ImageView> mPresentImageViews;

    std::vector<vk::Image> mPresentImages;
    vk::Image mDepthImage;
    vk::ImageView mDepthImageView;

    vk::RenderPass mRenderPass;

    std::vector<vk::Framebuffer> mFrameBuffers;
    
    vk::Buffer mUniformBuffer;
    vk::DeviceMemory mUniformBufferMemory;
    vk::DescriptorBufferInfo mUniformBufferInfo;

    vk::Pipeline mPipeline;
    vk::PipelineLayout mPipelineLayout;

    std::vector<vk::DescriptorSet> mDescriptorSets;
    vk::DescriptorSetLayout mDescriptorSetLayout;
    vk::DescriptorPool mDescriptorPool;

    std::vector<vk::WriteDescriptorSet> mWriteDescriptorSet;

    std::vector<Texture> mTextures;
  };
}