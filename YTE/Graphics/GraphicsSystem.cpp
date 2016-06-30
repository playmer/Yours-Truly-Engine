#include <iostream>

#include "Windows.h"

#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"

#include "YTE/Platform/Window.hpp"
#include "YTE/Platform/Windows/WindowData_Windows.hpp"

#include "vulkan/vkel.h"
#include "vulkan/vk_cpp.hpp"


namespace YTE
{
  class vulkan_context
  {
    public:
    u32 mWidth;
    u32 mHeight;

    vk::Instance mInstance;
    vk::SurfaceKHR mSurface;
    vk::DebugReportCallbackEXT mCallback;
    vk::PhysicalDevice mPhysicalDevice;
    vk::PhysicalDeviceProperties mPhysicalDeviceProperties;
    i32 mPresentQueueIdx;
    vk::Device mLogicalDevice;
    vk::SwapchainKHR mSwapChain;

    vk::Queue mQueue;
    vk::CommandBuffer mSetupCommandBuffer;
    vk::CommandBuffer mDrawCommandBuffer;

    std::vector<vk::Image> mPresentImages;
  };


  void vulkan_assert(u64 flag, char *msg = "")
  {
    if (!flag)
    {
      printf("ASSERT: ");
      printf("%s", msg);
      printf("\n");

      int *base = 0;
      *base = 1;
    }
  }

  void checkVulkanResult(vk::Result &aResult, char *aMessage) 
  {
    vulkan_assert(aResult == vk::Result::eSuccess, aMessage);
  }


  void checkVulkanResult(VkResult &aResult, char *aMessage)
  {
    vulkan_assert(aResult == VK_SUCCESS, aMessage);
  }

  enum class StringComparison
  {
    String1Null,     // (We check this first)
    LesserInString1,  // The first character that does not match has a lower value in ptr1 than in ptr2
    Equal,
    GreaterInString1,// The first character that does not match has a greater value in ptr1 than in ptr2
    String2Null,     // (We check this Second)
  };

  inline StringComparison StringCompare(const char *aLeft, const char *aRight)
  {
    if (nullptr == aLeft)
    {
      return StringComparison::String1Null;
    }

    if (nullptr == aRight)
    {
      return StringComparison::String2Null;
    }

    auto comparison = std::strcmp(aLeft, aRight);

    if (0 == comparison)
    {
      return StringComparison::Equal;
    }
    else if (comparison < 0)
    {
      return StringComparison::LesserInString1;
    }
    else // if (comparison < 0) This is by definition of the domain, no need to check
    {
      return StringComparison::GreaterInString1;
    }
  }



  VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkDebugReportFlagsEXT aFlags,
                                                     VkDebugReportObjectTypeEXT aObjectType, 
                                                     uint64_t aObject, 
                                                     size_t aLocation,
                                                     int32_t aMessageCode, 
                                                     const char *aLayerPrefix, 
                                                     const char *aMessage, 
                                                     void *aUserData) 
  {
    printf("%s", aLayerPrefix);
    printf(" ");
    printf("%s", aMessage);
    printf("\n");
    return VK_FALSE;
  }

  GraphicsSystem::GraphicsSystem(Engine *aEngine) : mEngine(aEngine), mVulkanSuccess(0)
  {
    auto self = mPlatformSpecificData.ConstructAndGet<vulkan_context>();
  }

  GraphicsSystem::~GraphicsSystem()
  {
    if (mVulkanSuccess)
    {
      vkelUninit();
    }
  }

  void GraphicsSystem::SetUpWindow(Window *aWindow)
  {
    if (mVulkanSuccess)
    {
    }
  }

  void GraphicsSystem::Initialize()
  {
    mVulkanSuccess = vkelInit();

    if (mVulkanSuccess)
    {
      auto self = mPlatformSpecificData.Get<vulkan_context>();

      auto appInfo = vk::ApplicationInfo()
        .setPApplicationName("First Test")
        .setEngineVersion(1)
        .setApiVersion(VK_MAKE_VERSION(1, 0, 0));

      auto instanceInfo = vk::InstanceCreateInfo()
        .setPApplicationInfo(&appInfo);

      auto layers = vk::enumerateInstanceLayerProperties();
      vulkan_assert(layers.size(), "Failed to find layers.");

      bool foundValidator = false;

      auto validationLayer = "VK_LAYER_LUNARG_standard_validation";

      for (auto &layer : layers)
      {
        std::cout << "Name: " << layer.layerName << "\n  Description: " << layer.description << "\n";

        if (StringCompare(layer.layerName, validationLayer) == StringComparison::Equal)
        {
          foundValidator = true;
        }
      }

      if (foundValidator)
      {
      }

      const char *enabledLayers[] = { validationLayer };

      if (foundValidator)
      {
        printf("Found the Standard Validator. \n");

        instanceInfo.setEnabledLayerCount(1);
        instanceInfo.setPpEnabledLayerNames(enabledLayers);
      }
      else
      {
          printf("Could not find validation layer.");
      }

      auto extensions = vk::enumerateInstanceExtensionProperties();

      std::array<const char *, 3> requiredExtensions = {
        "VK_KHR_surface",
        "VK_KHR_win32_surface",
        "VK_EXT_debug_report"
      };

      decltype(requiredExtensions)::size_type foundExtensions = 0;

      for (auto &extension : extensions)
      {
        for (auto &requiredExtension : requiredExtensions)
        {
          if (std::strcmp(extension.extensionName, requiredExtension) == 0)
          {
            ++foundExtensions;
            requiredExtension = "";
            break;
          }
        }
      }

      requiredExtensions = {
          "VK_KHR_surface",
          "VK_KHR_win32_surface",
          "VK_EXT_debug_report"
      };


      if (requiredExtensions.size() == foundExtensions)
      {
        instanceInfo.setEnabledExtensionCount((u32)requiredExtensions.size());
        instanceInfo.setPpEnabledExtensionNames(requiredExtensions.data());

        auto result = vk::createInstance(&instanceInfo, NULL, &self->mInstance);
        checkVulkanResult(result, "Failed to create vulkan instance.");
      }
      else
      {
        printf("Could not find debug extension");
      }

      vkelInstanceInit(self->mInstance);

      if (requiredExtensions.size() == foundExtensions)
      {
        auto  callbackCreateInfo = vk::DebugReportCallbackCreateInfoEXT()
                                        .setFlags(vk::DebugReportFlagBitsEXT::eError |
                                                  vk::DebugReportFlagBitsEXT::eWarning |
                                                  vk::DebugReportFlagBitsEXT::ePerformanceWarning)
                                        .setPfnCallback(&DebugReportCallback);
        
        auto debugReport = self->mInstance.createDebugReportCallbackEXT(callbackCreateInfo);
        vulkan_assert(static_cast<bool>(debugReport), "Failed to create degub report callback.");
      }

      // TODO: Abstract this for multiple windows.
      auto window = mEngine->mPrimaryWindow;
      auto windowData = window->mPlatformSpecificData.Get<WindowData>();
      auto surfaceCreateInfo = vk::Win32SurfaceCreateInfoKHR()
                                    .setHinstance(windowData->mInstance)
                                    .setHwnd(windowData->mWindowHandle);

      self->mHeight = window->mHeight;
      self->mWidth = window->mWidth;

      self->mSurface = self->mInstance.createWin32SurfaceKHR(surfaceCreateInfo);

      auto physicalDevices = self->mInstance.enumeratePhysicalDevices();

      for (auto &physicalDevice : physicalDevices) 
      {
        auto properties = physicalDevice.getProperties();

        auto queueProperties = physicalDevice.getQueueFamilyProperties();

        u32 queueCount = 0;
        for (auto &queueProperty : queueProperties)
        {
          i32 supportsPresent = physicalDevice.getSurfaceSupportKHR(queueCount, self->mSurface);

          if (supportsPresent && (queueProperty.queueFlags & vk::QueueFlagBits::eGraphics))
          {
            self->mPhysicalDevice = physicalDevice;
            self->mPhysicalDeviceProperties = properties;
            self->mPresentQueueIdx = queueCount;

            ++queueCount;
            break;
          }

          ++queueCount;
        }


        // TODO: Maybe grab more than just one physical device?
        if (self->mPhysicalDevice) 
        {
          break;
        }
      }

      vulkan_assert((bool)self->mPhysicalDevice, "No physical device detected that can render and present!");

      // info for accessing one of the devices rendering queues:
      auto queueCreateInfo = vk::DeviceQueueCreateInfo()
                                  .setQueueFamilyIndex(self->mPresentQueueIdx)
                                  .setQueueCount(1);

      float queuePriorities[] = { 1.0f };   // ask for highest priority for our queue. (range [0,1])
      queueCreateInfo.pQueuePriorities = queuePriorities;

      const char *deviceExtensions[] = { "VK_KHR_swapchain" };

      auto deviceInfo = vk::DeviceCreateInfo()
                             .setQueueCreateInfoCount(1)
                             .setPQueueCreateInfos(&queueCreateInfo)
                             .setEnabledLayerCount(1)
                             .setPpEnabledLayerNames(enabledLayers)
                             .setEnabledExtensionCount(1)
                             .setPpEnabledExtensionNames(deviceExtensions);

      auto features = vk::PhysicalDeviceFeatures().setShaderClipDistance(true);
      
      deviceInfo.setPEnabledFeatures(&features);

      self->mLogicalDevice = self->mPhysicalDevice.createDevice(deviceInfo);

      vulkan_assert((bool)self->mLogicalDevice, "Failed to create logical device!");

      auto formats = self->mPhysicalDevice.getSurfaceFormatsKHR(self->mSurface);

      // If the format list includes just one entry of VK_FORMAT_UNDEFINED, the surface has
      // no preferred format. Otherwise, at least one supported format will be returned.
      vk::Format colorFormat;

      if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined)
      {
        colorFormat = vk::Format::eB8G8R8Unorm;
      }
      else 
      {
        colorFormat = formats[0].format;
      }

      vk::ColorSpaceKHR colorSpace;
      colorSpace = formats[0].colorSpace;

      vk::SurfaceCapabilitiesKHR surfaceCapabilities = self->mPhysicalDevice.getSurfaceCapabilitiesKHR(self->mSurface);

      // We are effectively looking for double-buffering:
      // if surfaceCapabilities.maxImageCount == 0 there is actually no limit on the number of images! 
      //
      // TODO: Make the amount of buffering configurable.
      u32 desiredImageCount = 2;

      if (desiredImageCount < surfaceCapabilities.minImageCount) 
      {
        desiredImageCount = surfaceCapabilities.minImageCount;
      }
      else if ((surfaceCapabilities.maxImageCount != 0) && (desiredImageCount > surfaceCapabilities.maxImageCount)) 
      {
        desiredImageCount = surfaceCapabilities.maxImageCount;
      }

      vk::Extent2D surfaceResolution = surfaceCapabilities.currentExtent;

      if (surfaceResolution.width == -1) 
      {
        surfaceResolution.width = self->mWidth;
        surfaceResolution.height = self->mHeight;
      }
      else 
      {
        self->mWidth = surfaceResolution.width;
        self->mHeight = surfaceResolution.height;
      }

      vk::SurfaceTransformFlagBitsKHR preTransform = surfaceCapabilities.currentTransform;

      if (surfaceCapabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity)
      {
        preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
      }

      auto presentModes = self->mPhysicalDevice.getSurfacePresentModesKHR(self->mSurface);

      // Always supported.
      vk::PresentModeKHR presentationMode = vk::PresentModeKHR::eFifo;   

      // TODO: Look into the rest of the vk::PresentModeKHR options.
      for (auto &presentMode : presentModes) 
      {
        if (presentMode == vk::PresentModeKHR::eMailbox)
        {
          presentationMode = vk::PresentModeKHR::eMailbox;
          break;
        }
      }

      auto swapChainCreateInfo = vk::SwapchainCreateInfoKHR()
                                      .setSurface(self->mSurface)
                                      .setMinImageCount(desiredImageCount)
                                      .setImageFormat(colorFormat)
                                      .setImageColorSpace(colorSpace)
                                      .setImageExtent(surfaceResolution)
                                      .setImageArrayLayers(1)
                                      .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
                                      .setImageSharingMode(vk::SharingMode::eExclusive)   // TODO: Learn how to share queues.
                                      .setPreTransform(preTransform)
                                      .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque) //TODO: Pretty sure we want to do pre-multiplied.
                                      .setPresentMode(presentationMode)
                                      .setClipped(true); // If we want clipping outside the extents
                                                         // (remember our device features?)

      self->mSwapChain = self->mLogicalDevice.createSwapchainKHR(swapChainCreateInfo);
      vulkan_assert((bool)self->mSwapChain, "Failed to create swapchain.");

      self->mQueue = self->mLogicalDevice.getQueue(self->mPresentQueueIdx, 0);

      auto commandPoolCreateInfo = vk::CommandPoolCreateInfo()
                                        .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                                        .setQueueFamilyIndex(self->mPresentQueueIdx);

      auto commandPool = self->mLogicalDevice.createCommandPool(commandPoolCreateInfo);
      vulkan_assert((bool)commandPool, "Failed to create command pool.");

      auto commandBufferAllocationInfo = vk::CommandBufferAllocateInfo()
                                              .setCommandPool(commandPool)
                                              .setLevel(vk::CommandBufferLevel::ePrimary)
                                              .setCommandBufferCount(1);

      self->mSetupCommandBuffer = self->mLogicalDevice.allocateCommandBuffers(commandBufferAllocationInfo)[0];
      vulkan_assert((bool)self->mSetupCommandBuffer, "Failed to allocate setup command buffer.");

      self->mDrawCommandBuffer = self->mLogicalDevice.allocateCommandBuffers(commandBufferAllocationInfo)[0];
      vulkan_assert((bool)self->mDrawCommandBuffer, "Failed to allocate draw command buffer.");

      self->mPresentImages = self->mLogicalDevice.getSwapchainImagesKHR(self->mSwapChain);

      auto presentImagesViewCreateInfo = vk::ImageViewCreateInfo()
                                              .setViewType(vk::ImageViewType::e2D)
                                              .setFormat(colorFormat)
                                              .setComponents({ vk::ComponentSwizzle::eR,
                                                               vk::ComponentSwizzle::eG,
                                                               vk::ComponentSwizzle::eB,
                                                               vk::ComponentSwizzle::eA });

      presentImagesViewCreateInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
      presentImagesViewCreateInfo.subresourceRange.setLevelCount(1);
      presentImagesViewCreateInfo.subresourceRange.setLayerCount(1);

      auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

      auto fenceCreateInfo = vk::FenceCreateInfo();

      auto submitFence = self->mLogicalDevice.createFence(fenceCreateInfo);

      std::vector<bool> transitioned;
      transitioned.resize(self->mPresentImages.size(), false);

      u32 doneCount = 0;
      while (doneCount != self->mPresentImages.size())
      {
        vk::SemaphoreCreateInfo semaphoreCreateInfo;
        vk::Semaphore presentCompleteSemaphore = self->mLogicalDevice.createSemaphore(semaphoreCreateInfo);

        u32 nextImageIdx;
        auto result = self->mLogicalDevice.acquireNextImageKHR(self->mSwapChain, UINT64_MAX, presentCompleteSemaphore, VK_NULL_HANDLE, &nextImageIdx);
        checkVulkanResult(result, "Could not acquireNextImageKHR.");

        if (!transitioned.at(nextImageIdx))
        {
          // start recording out image layout change barrier on our setup command buffer:
          self->mSetupCommandBuffer.begin(&beginInfo);

          auto layoutTransitionBarrier = vk::ImageMemoryBarrier()
                                              .setDstAccessMask(vk::AccessFlagBits::eMemoryRead)
                                              .setOldLayout(vk::ImageLayout::eUndefined)
                                              .setNewLayout(vk::ImageLayout::ePresentSrcKHR)
                                              .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                                              .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                                              .setImage(self->mPresentImages[nextImageIdx]);

          VkImageSubresourceRange resourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
          layoutTransitionBarrier.subresourceRange = resourceRange;

          self->mSetupCommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, 
                                                    vk::PipelineStageFlagBits::eTopOfPipe, 
                                                    vk::DependencyFlags(), 
                                                    nullptr, 
                                                    nullptr,
                                                    layoutTransitionBarrier);

          self->mSetupCommandBuffer.end();

          vk::PipelineStageFlags waitStageMash[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
          auto submitInfo = vk::SubmitInfo()
                                 .setWaitSemaphoreCount(1)
                                 .setPWaitSemaphores(&presentCompleteSemaphore)
                                 .setPWaitDstStageMask(waitStageMash)
                                 .setCommandBufferCount(1)
                                 .setPCommandBuffers(&self->mSetupCommandBuffer);

          self->mQueue.submit(submitInfo, submitFence);
          
          self->mLogicalDevice.waitForFences(submitFence, true, UINT64_MAX);
          self->mLogicalDevice.resetFences(submitFence);

          self->mLogicalDevice.destroySemaphore(presentCompleteSemaphore);

          // NOTE: Instead of eReleaseResources should it be 0?
          self->mSetupCommandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);

          transitioned[nextImageIdx] = true;
          ++doneCount;
        }

        auto presentInfo = vk::PresentInfoKHR()
                                .setSwapchainCount(1)
                                .setPSwapchains(&self->mSwapChain)
                                .setPImageIndices(&nextImageIdx);

        self->mQueue.presentKHR(presentInfo);
      }
    }
  }

  void GraphicsSystem::Render()
  {
    auto self = mPlatformSpecificData.Get<vulkan_context>();

    u32 nextImageIdx;
    auto result = self->mLogicalDevice.acquireNextImageKHR(self->mSwapChain, UINT64_MAX, VK_NULL_HANDLE, VK_NULL_HANDLE, &nextImageIdx);
    
    auto presentInfo = vk::PresentInfoKHR()
                            .setSwapchainCount(1)
                            .setPSwapchains(&self->mSwapChain)
                            .setPImageIndices(&nextImageIdx);

    self->mQueue.presentKHR(presentInfo);
  }

  void GraphicsSystem::Update(float aDt)
  {
    if (mVulkanSuccess)
    {
      Render();
    }

    for (auto &window : mEngine->mWindows)
    {
      window->SwapBuffers();
    }
  }
}
