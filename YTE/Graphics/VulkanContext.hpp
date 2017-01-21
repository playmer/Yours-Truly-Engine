#pragma once

#include "YTE/Core/Delegate.hpp"
#include "YTE/Core/Types.hpp"

#include "YTE/Graphics/Texture.hpp"
#include "YTE/Graphics/Material.hpp"
#include "YTE/Graphics/View.hpp"
#include "YTE/Graphics/VulkanPrimitives.hpp"

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

  class VulkanContext
  {
  public:
    VulkanContext()
      : mView(this)
    {
    }

    ~VulkanContext()
    {
    }

    template<typename FlagType>
    u32 GetMemoryType(u32 aTypeBits, FlagType aProperties)
    {
      for (uint32_t i = 0; i < 32; i++)
      {
        if ((aTypeBits & 1) == 1)
        {
          auto flags = mPhysicalMemoryProperties.memoryTypes[i].propertyFlags;

          if ((flags & aProperties) == aProperties)
          {
            return i;
          }
        }
        aTypeBits >>= 1;
      }

      // todo : throw error
      return 0;
    }

    BufferMemory CreateBuffer(vk::DeviceSize size, 
                              vk::BufferUsageFlags usage, 
                              vk::MemoryPropertyFlags properties);
    void CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

    template <typename Type>
    BufferMemory CreateFilledBuffer(const Type *aData, 
                                    u64 aSize, 
                                    bool aUseStaging = true)
    {
      vk::DeviceSize bufferSize = static_cast<u32>(sizeof(Type) * aSize);

      BufferMemory toReturn(&mLogicalDevice);

      auto bufferMemory = CreateBuffer(bufferSize,
                                       vk::BufferUsageFlagBits::eTransferSrc, 
                                       vk::MemoryPropertyFlagBits::eHostVisible | 
                                       vk::MemoryPropertyFlagBits::eHostCoherent);

      void *data = mLogicalDevice.mapMemory(bufferMemory.mMemory, 0, bufferSize);
      memcpy(data, aData, (size_t)bufferSize);
      mLogicalDevice.unmapMemory(bufferMemory.mMemory);

      if (aUseStaging)
      {
        auto staging = CreateBuffer(bufferSize, 
                                    vk::BufferUsageFlagBits::eTransferDst | 
                                    vk::BufferUsageFlagBits::eVertexBuffer, 
                                    vk::MemoryPropertyFlagBits::eDeviceLocal);

        CopyBuffer(bufferMemory.mBuffer, staging.mBuffer, bufferSize);

        toReturn = std::move(staging);
      }
      else
      {
        toReturn = std::move(bufferMemory);
      }

      return toReturn;
    }

    template <typename Type>
    BufferMemory CreateFilledBuffer(std::initializer_list<Type> aData, 
                                    bool aUseStaging = true)
    {
      return CreateFilledBuffer(aData.begin(), aData.size(), aUseStaging);
    }

    template <typename Type>
    BufferMemory CreateFilledBuffer(std::vector<Type> aData, 
                                    bool aUseStaging = true)
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
    

    std::vector<vk::DescriptorSet> mDescriptorSets;
    vk::DescriptorPool mDescriptorPool;

    std::vector<Texture> mTextures;


    std::vector<Material> mMaterials;
    std::vector<vk::WriteDescriptorSet> mWriteDescriptorSet;

    View mView;
  };
}