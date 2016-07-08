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
  struct vertex 
  {
    float x, y, z, w;
  };


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
    vk::PhysicalDeviceMemoryProperties mPhysicalMemoryProperties;

    i32 mPresentQueueIdx;
    vk::Device mLogicalDevice;
    vk::SwapchainKHR mSwapChain;

    vk::Queue mQueue;
    vk::CommandBuffer mSetupCommandBuffer;
    vk::CommandBuffer mDrawCommandBuffer;
    std::vector<vk::ImageView> mPresentImageViews;

    std::vector<vk::Image> mPresentImages;
    vk::Image mDepthImage;
    vk::ImageView mDepthImageView;

    vk::RenderPass mRenderPass;

    std::vector<vk::Framebuffer> mFrameBuffers;

    vk::Buffer mVertexInputBuffer;

    vk::Pipeline mPipeline;
    vk::PipelineLayout mPipelineLayout;
  };


  inline void vulkan_actual_assert(u64 flag, char *msg = "")
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

  inline void vulkan_assert(u64 flag, char *msg = "")
  {
    vulkan_actual_assert(flag, msg);
  }

  inline void vulkan_assert(void *flag, char *msg = "")
  {
      vulkan_actual_assert(reinterpret_cast<u64>(flag), msg);
  }

  template<typename Type>
  inline void vulkan_assert(Type flag, char *msg = "")
  {
    vulkan_actual_assert(static_cast<bool>(flag), msg);
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

      vulkan_assert(self->mPhysicalDevice, "No physical device detected that can render and present!");

      self->mPhysicalMemoryProperties = self->mPhysicalDevice.getMemoryProperties();

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

      vulkan_assert(self->mLogicalDevice, "Failed to create logical device!");

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
      vulkan_assert(self->mSwapChain, "Failed to create swapchain.");

      self->mQueue = self->mLogicalDevice.getQueue(self->mPresentQueueIdx, 0);

      auto commandPoolCreateInfo = vk::CommandPoolCreateInfo()
                                        .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                                        .setQueueFamilyIndex(self->mPresentQueueIdx);

      auto commandPool = self->mLogicalDevice.createCommandPool(commandPoolCreateInfo);
      vulkan_assert(commandPool, "Failed to create command pool.");

      auto commandBufferAllocationInfo = vk::CommandBufferAllocateInfo()
                                              .setCommandPool(commandPool)
                                              .setLevel(vk::CommandBufferLevel::ePrimary)
                                              .setCommandBufferCount(1);

      self->mSetupCommandBuffer = self->mLogicalDevice.allocateCommandBuffers(commandBufferAllocationInfo)[0];
      vulkan_assert(self->mSetupCommandBuffer, "Failed to allocate setup command buffer.");

      self->mDrawCommandBuffer = self->mLogicalDevice.allocateCommandBuffers(commandBufferAllocationInfo)[0];
      vulkan_assert(self->mDrawCommandBuffer, "Failed to allocate draw command buffer.");

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

      auto submitFence = self->mLogicalDevice.createFence(vk::FenceCreateInfo());

      std::vector<bool> transitioned;
      transitioned.resize(self->mPresentImages.size(), false);


      // This sets the image layout on the images.
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
      
          vk::ImageSubresourceRange resourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };
          layoutTransitionBarrier.setSubresourceRange(resourceRange);
      
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

        // TODO: Fix the issue where we present without writing to the images.
        auto presentInfo = vk::PresentInfoKHR()
                                .setSwapchainCount(1)
                                .setPSwapchains(&self->mSwapChain)
                                .setPImageIndices(&nextImageIdx);
        
        self->mQueue.presentKHR(presentInfo);
      }

      self->mPresentImageViews = std::vector<vk::ImageView>(self->mPresentImages.size(), vk::ImageView());

      for (uint32_t i = 0; i < self->mPresentImages.size(); ++i)
      {
        presentImagesViewCreateInfo.image = self->mPresentImages[i];

        self->mPresentImageViews[i] = self->mLogicalDevice.createImageView(presentImagesViewCreateInfo);

        vulkan_assert(self->mPresentImageViews[i], "Could not create ImageView.");
      }


      auto imageCreateInfo = vk::ImageCreateInfo()
                                  .setImageType(vk::ImageType::e2D)
                                  .setFormat(vk::Format::eD16Unorm)
                                  .setExtent({ self->mWidth, self->mHeight, 1 })
                                  .setMipLevels(1)
                                  .setArrayLayers(1)
                                  .setSamples(vk::SampleCountFlagBits::e1)
                                  .setTiling(vk::ImageTiling::eOptimal)
                                  .setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment)
                                  .setSharingMode(vk::SharingMode::eExclusive) // TODO: Change this when you learn more.
                                  .setInitialLayout(vk::ImageLayout::eUndefined);

      self->mDepthImage = self->mLogicalDevice.createImage(imageCreateInfo);

      vulkan_assert(self->mDepthImage, "Failed to create depth image.");

      auto memoryRequirements = self->mLogicalDevice.getImageMemoryRequirements(self->mDepthImage);
      auto imageAllocationInfo = vk::MemoryAllocateInfo().setAllocationSize(memoryRequirements.size);
      
      // memoryTypeBits is a bitfield where if bit i is set, it means that 
      // the VkMemoryType i of the VkPhysicalDeviceMemoryProperties structure 
      // satisfies the memory requirements:
      u32 memoryTypeBits = memoryRequirements.memoryTypeBits;
      vk::MemoryPropertyFlags desiredMemoryFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
      for (u32 i = 0; i < 32; ++i) 
      {
        vk::MemoryType memoryType = self->mPhysicalMemoryProperties.memoryTypes[i];
        if (memoryTypeBits & 1) 
        {
          if ((memoryType.propertyFlags & desiredMemoryFlags) == desiredMemoryFlags) 
          {
            imageAllocationInfo.memoryTypeIndex = i;
            break;
          }
        }
        memoryTypeBits = memoryTypeBits >> 1;
      }

      auto imageMemory = self->mLogicalDevice.allocateMemory(imageAllocationInfo);
      self->mLogicalDevice.bindImageMemory(self->mDepthImage, imageMemory, 0);

      self->mSetupCommandBuffer.begin(beginInfo);
      auto layoutTransitionBarrier = vk::ImageMemoryBarrier()
                                          .setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentRead |
                                                            vk::AccessFlagBits::eDepthStencilAttachmentWrite)
                                          .setOldLayout(vk::ImageLayout::eUndefined)
                                          .setNewLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
                                          .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                                          .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                                          .setImage(self->mDepthImage);

      auto resourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1);

      layoutTransitionBarrier.setSubresourceRange(resourceRange);

      self->mSetupCommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                                                vk::PipelineStageFlagBits::eTopOfPipe,
                                                vk::DependencyFlags(),
                                                nullptr,
                                                nullptr,
                                                layoutTransitionBarrier);

      self->mSetupCommandBuffer.end();

      vk::PipelineStageFlags waitStageMask[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
      auto submitInfo = vk::SubmitInfo()
                             .setPWaitDstStageMask(waitStageMask)
                             .setCommandBufferCount(1)
                             .setPCommandBuffers(&self->mSetupCommandBuffer);

      self->mQueue.submit(submitInfo, submitFence);

      self->mLogicalDevice.waitForFences(submitFence, true, UINT64_MAX);
      self->mLogicalDevice.resetFences(submitFence);

      // NOTE: Instead of eReleaseResources should it be 0?
      self->mSetupCommandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);

      vk::ImageAspectFlags aspectMask = vk::ImageAspectFlagBits::eDepth;
      auto imageViewCreateInfo = vk::ImageViewCreateInfo()
                                      .setImage(self->mDepthImage)
                                      .setViewType(vk::ImageViewType::e2D)
                                      .setFormat(imageCreateInfo.format)
                                      .setComponents({vk::ComponentSwizzle::eIdentity, 
                                                      vk::ComponentSwizzle::eIdentity, 
                                                      vk::ComponentSwizzle::eIdentity , 
                                                      vk::ComponentSwizzle::eIdentity })
                                      .setSubresourceRange(vk::ImageSubresourceRange(aspectMask, 0, 1, 0, 1));

      self->mDepthImageView = self->mLogicalDevice.createImageView(imageViewCreateInfo);

      vulkan_assert(self->mDepthImageView, "Failed to create image view.");

      vk::AttachmentDescription passAttachments[2] = {};
      passAttachments[0].format = colorFormat;
      passAttachments[0].samples = vk::SampleCountFlagBits::e1;
      passAttachments[0].loadOp = vk::AttachmentLoadOp::eClear;
      passAttachments[0].storeOp = vk::AttachmentStoreOp::eStore;
      passAttachments[0].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
      passAttachments[0].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
      passAttachments[0].initialLayout = vk::ImageLayout::eColorAttachmentOptimal;
      passAttachments[0].finalLayout = vk::ImageLayout::eColorAttachmentOptimal;

      passAttachments[1].format = vk::Format::eD16Unorm;
      passAttachments[1].samples = vk::SampleCountFlagBits::e1;
      passAttachments[1].loadOp = vk::AttachmentLoadOp::eClear;
      passAttachments[1].storeOp = vk::AttachmentStoreOp::eDontCare;
      passAttachments[1].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
      passAttachments[1].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
      passAttachments[1].initialLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
      passAttachments[1].finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

      auto colorAttachmentReference = vk::AttachmentReference()
                                           .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

      auto depthAttachmentReference = vk::AttachmentReference()
                                           .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

      auto subpass = vk::SubpassDescription()
                          .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
                          .setColorAttachmentCount(1)
                          .setPColorAttachments(&colorAttachmentReference)
                          .setPDepthStencilAttachment(&depthAttachmentReference);

      auto renderPassCreateInfo = vk::RenderPassCreateInfo()
                                       .setAttachmentCount(2)
                                       .setPAttachments(passAttachments)
                                       .setSubpassCount(1)
                                       .setPSubpasses(&subpass);

      self->mRenderPass = self->mLogicalDevice.createRenderPass(renderPassCreateInfo);
      vulkan_assert(self->mRenderPass, "Failed to create renderpass");

      vk::ImageView frameBufferAttachments[2];
      frameBufferAttachments[1] = self->mDepthImageView;

      auto frameBufferCreateInfo = vk::FramebufferCreateInfo()
                                        .setRenderPass(self->mRenderPass)
                                        .setAttachmentCount(2)  // must be equal to the attachment count on render pass
                                        .setPAttachments(frameBufferAttachments)
                                        .setWidth(self->mWidth)
                                        .setHeight(self->mHeight)
                                        .setLayers(1);

      // create a framebuffer per swap chain imageView:
      auto imageCount = self->mPresentImages.size();
      self->mFrameBuffers = std::vector<vk::Framebuffer>(imageCount, vk::Framebuffer());

      u32 i = 0;
      for (auto &frameBuffer : self->mFrameBuffers)
      {
        frameBufferAttachments[0] = self->mPresentImageViews[i];
        frameBuffer = self->mLogicalDevice.createFramebuffer(frameBufferCreateInfo);

        vulkan_assert(frameBuffer, "Failed to create framebuffer.");

        ++i;
      }

      // Create our vertex buffer:
      auto vertexInputBufferInfo = vk::BufferCreateInfo()
                                        .setSize(sizeof(vertex) * 3) // Size in bytes.
                                        .setUsage(vk::BufferUsageFlagBits::eVertexBuffer)
                                        .setSharingMode(vk::SharingMode::eExclusive); // TODO: Change to not exclusive.

      self->mVertexInputBuffer = self->mLogicalDevice.createBuffer(vertexInputBufferInfo);

      vulkan_assert(self->mVertexInputBuffer, "Failed to create vertex input buffer.");

      auto vertexBufferMemoryRequirements = self->mLogicalDevice.getBufferMemoryRequirements(self->mVertexInputBuffer);

      auto bufferAllocateInfo = vk::MemoryAllocateInfo()
                                     .setAllocationSize(vertexBufferMemoryRequirements.size);

      u32 vertexMemoryTypeBits = vertexBufferMemoryRequirements.memoryTypeBits;
      vk::MemoryPropertyFlags vertexDesiredMemoryFlags = vk::MemoryPropertyFlagBits::eHostVisible;

      i = 0;
      for (auto &memoryType : self->mPhysicalMemoryProperties.memoryTypes)
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

      auto vertexBufferMemory =  self->mLogicalDevice.allocateMemory(bufferAllocateInfo);
      vulkan_assert(vertexBufferMemory, "Failed to allocate buffer memory.");

      void *mapped = self->mLogicalDevice.mapMemory(vertexBufferMemory, 0, VK_WHOLE_SIZE);
      vulkan_assert(mapped, "Failed to map buffer memory.");

      vertex *triangle = (vertex *)mapped;
      vertex v1 = { -1.0f, -1.0f, 0, 1.0f };
      vertex v2 = { 1.0f, -1.0f, 0, 1.0f };
      vertex v3 = { 0.0f,  1.0f, 0, 1.0f };
      triangle[0] = v1;
      triangle[1] = v2;
      triangle[2] = v3;

      self->mLogicalDevice.unmapMemory(vertexBufferMemory);

      self->mLogicalDevice.bindBufferMemory(self->mVertexInputBuffer, vertexBufferMemory, 0);

      uint32_t codeSize;
      char *code = new char[10000];
      HANDLE fileHandle = 0;

      // load our vertex shader:
      fileHandle = CreateFile("./Shaders/vert.spv", GENERIC_READ, 0, NULL,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
      if (fileHandle == INVALID_HANDLE_VALUE) 
      {
        OutputDebugStringA("Failed to open shader file.");
        exit(1);
      }

      ReadFile((HANDLE)fileHandle, code, 10000, (LPDWORD)&codeSize, 0);
      CloseHandle(fileHandle);

      auto vertexShaderCreationInfo = vk::ShaderModuleCreateInfo()
                                           .setCodeSize(codeSize)
                                           .setPCode((uint32_t *)code);

      auto vertexShaderModule = self->mLogicalDevice.createShaderModule(vertexShaderCreationInfo);
      vulkan_assert(vertexShaderModule, "Failed to create vertex shader module.");

      // load our fragment shader:
      fileHandle = CreateFile("./Shaders/frag.spv", GENERIC_READ, 0, NULL,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
      if (fileHandle == INVALID_HANDLE_VALUE) 
      {
        OutputDebugStringA("Failed to open shader file.");
        exit(1);
      }
      ReadFile((HANDLE)fileHandle, code, 10000, (LPDWORD)&codeSize, 0);
      CloseHandle(fileHandle);

      auto fragmentShaderCreationInfo = vk::ShaderModuleCreateInfo()
                                            .setCodeSize(codeSize)
                                            .setPCode((uint32_t *)code);
      
      auto fragmentShaderModule = self->mLogicalDevice.createShaderModule(fragmentShaderCreationInfo);
      vulkan_assert(fragmentShaderModule, "Failed to create fragment shader module.");

      vk::PipelineLayoutCreateInfo layoutCreateInfo = {};
      layoutCreateInfo.setSetLayoutCount(0);

      self->mPipelineLayout = self->mLogicalDevice.createPipelineLayout(layoutCreateInfo);
      vulkan_assert(self->mPipelineLayout, "Failed to create pipeline layout.");

      vk::PipelineShaderStageCreateInfo shaderStageCreateInfo[2];
      shaderStageCreateInfo[0].stage = vk::ShaderStageFlagBits::eVertex;
      shaderStageCreateInfo[0].module = vertexShaderModule;
      shaderStageCreateInfo[0].pName = "main";        // shader entry point function name
      shaderStageCreateInfo[0].pSpecializationInfo = NULL;

      shaderStageCreateInfo[1].stage = vk::ShaderStageFlagBits::eFragment;
      shaderStageCreateInfo[1].module = fragmentShaderModule;
      shaderStageCreateInfo[1].pName = "main";        // shader entry point function name
      shaderStageCreateInfo[1].pSpecializationInfo = NULL;

      vk::VertexInputBindingDescription vertexBindingDescription;
      vertexBindingDescription.stride = sizeof(vertex);
      vertexBindingDescription.inputRate = vk::VertexInputRate::eVertex;

      vk::VertexInputAttributeDescription vertexAttributeDescritpion;
      vertexAttributeDescritpion.location = 0;
      vertexAttributeDescritpion.binding = 0;
      vertexAttributeDescritpion.format = vk::Format::eR32G32B32A32Sfloat;
      vertexAttributeDescritpion.offset = 0;

      vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
      vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
      vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
      vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 1;
      vertexInputStateCreateInfo.pVertexAttributeDescriptions = &vertexAttributeDescritpion;

      // vertex topology config:
      vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
      inputAssemblyStateCreateInfo.topology = vk::PrimitiveTopology::eTriangleList;

      vk::Viewport viewport = {};
      viewport.width = static_cast<float>(self->mWidth);
      viewport.height = static_cast<float>(self->mHeight);
      viewport.maxDepth = 1;

      vk::Rect2D scissors = {};
      scissors.extent = { self->mWidth, self->mHeight };

      vk::PipelineViewportStateCreateInfo viewportState = {};
      viewportState.viewportCount = 1;
      viewportState.pViewports = &viewport;
      viewportState.scissorCount = 1;
      viewportState.pScissors = &scissors;

      vk::PipelineRasterizationStateCreateInfo rasterizationState;
      rasterizationState.polygonMode = vk::PolygonMode::eFill;
      rasterizationState.cullMode = vk::CullModeFlagBits::eNone;
      rasterizationState.frontFace = vk::FrontFace::eCounterClockwise;
      rasterizationState.lineWidth = 1;

      vk::PipelineMultisampleStateCreateInfo multisampleState = {};
      multisampleState.rasterizationSamples = vk::SampleCountFlagBits::e1;

      vk::StencilOpState noOPStencilState = {};
      noOPStencilState.failOp = vk::StencilOp::eKeep;
      noOPStencilState.passOp = vk::StencilOp::eKeep;
      noOPStencilState.depthFailOp = vk::StencilOp::eKeep;
      noOPStencilState.compareOp = vk::CompareOp::eAlways;

      vk::PipelineDepthStencilStateCreateInfo depthState;
      depthState.depthTestEnable = true;
      depthState.depthWriteEnable = true;
      depthState.depthCompareOp = vk::CompareOp::eLessOrEqual;
      depthState.front = noOPStencilState;
      depthState.back = noOPStencilState;

      vk::PipelineColorBlendAttachmentState colorBlendAttachmentState;
      colorBlendAttachmentState.srcColorBlendFactor = vk::BlendFactor::eSrc1Color;
      colorBlendAttachmentState.dstColorBlendFactor = vk::BlendFactor::eOneMinusDstColor;
      colorBlendAttachmentState.colorBlendOp = vk::BlendOp::eAdd;
      colorBlendAttachmentState.srcAlphaBlendFactor = vk::BlendFactor::eZero;
      colorBlendAttachmentState.dstAlphaBlendFactor = vk::BlendFactor::eZero;
      colorBlendAttachmentState.alphaBlendOp = vk::BlendOp::eAdd;
      colorBlendAttachmentState.colorWriteMask = vk::ColorComponentFlagBits::eR | 
                                                 vk::ColorComponentFlagBits::eG |
                                                 vk::ColorComponentFlagBits::eB |
                                                 vk::ColorComponentFlagBits::eA;

      vk::PipelineColorBlendStateCreateInfo colorBlendState;
      colorBlendState.logicOpEnable = VK_FALSE;
      colorBlendState.logicOp = vk::LogicOp::eClear;
      colorBlendState.attachmentCount = 1;
      colorBlendState.pAttachments = &colorBlendAttachmentState;
      colorBlendState.blendConstants[0] = 0.0;
      colorBlendState.blendConstants[1] = 0.0;
      colorBlendState.blendConstants[2] = 0.0;
      colorBlendState.blendConstants[3] = 0.0;

      vk::DynamicState dynamicState[2] = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
      vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo ;
      dynamicStateCreateInfo.dynamicStateCount = 2;
      dynamicStateCreateInfo.pDynamicStates = dynamicState;

      vk::GraphicsPipelineCreateInfo pipelineCreateInfo;
      pipelineCreateInfo.stageCount = 2;
      pipelineCreateInfo.pStages = shaderStageCreateInfo;
      pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
      pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
      pipelineCreateInfo.pViewportState = &viewportState;
      pipelineCreateInfo.pRasterizationState = &rasterizationState;
      pipelineCreateInfo.pMultisampleState = &multisampleState;
      pipelineCreateInfo.pDepthStencilState = &depthState;
      pipelineCreateInfo.pColorBlendState = &colorBlendState;
      pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
      pipelineCreateInfo.layout = self->mPipelineLayout;
      pipelineCreateInfo.renderPass = self->mRenderPass;

      self->mPipeline = self->mLogicalDevice.createGraphicsPipelines(VK_NULL_HANDLE, pipelineCreateInfo)[0];

      vulkan_assert(self->mPipeline, "Failed to create graphics pipeline.");
    }
  }

  void GraphicsSystem::VulkanRender()
  {
    auto self = mPlatformSpecificData.Get<vulkan_context>();

    vk::SemaphoreCreateInfo semaphoreCreateInfo = vk::SemaphoreCreateInfo();

    auto presentCompleteSemaphore = self->mLogicalDevice.createSemaphore(semaphoreCreateInfo);
    auto renderingCompleteSemaphore = self->mLogicalDevice.createSemaphore(semaphoreCreateInfo);

    u32 nextImageIdx;

    auto result = self->mLogicalDevice.acquireNextImageKHR(self->mSwapChain, UINT64_MAX, presentCompleteSemaphore, VK_NULL_HANDLE, &nextImageIdx);
    checkVulkanResult(result, "Could not acquireNextImageKHR.");

    vk::CommandBufferBeginInfo beginInfo = {};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    self->mDrawCommandBuffer.begin(beginInfo);

    // change image layout from VK_IMAGE_LAYOUT_PRESENT_SRC_KHR to VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    vk::ImageMemoryBarrier layoutTransitionBarrier = {};
    layoutTransitionBarrier.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
    layoutTransitionBarrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | 
                                            vk::AccessFlagBits::eColorAttachmentWrite;
    layoutTransitionBarrier.oldLayout = vk::ImageLayout::ePresentSrcKHR;
    layoutTransitionBarrier.newLayout = vk::ImageLayout::eColorAttachmentOptimal;
    layoutTransitionBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    layoutTransitionBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    layoutTransitionBarrier.image = self->mPresentImages[nextImageIdx];

    auto resourceRange = vk::ImageSubresourceRange( vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 );
    layoutTransitionBarrier.subresourceRange = resourceRange;

    self->mDrawCommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                                             vk::PipelineStageFlagBits::eTopOfPipe,
                                             vk::DependencyFlags(),
                                             nullptr,
                                             nullptr,
                                             layoutTransitionBarrier);

    // activate render pass:
    vk::ClearValue clearValue[2];
    clearValue[0].color.float32[0] = 1.0f;
    clearValue[0].color.float32[1] = 1.0f;
    clearValue[0].color.float32[2] = 1.0f;
    clearValue[0].color.float32[3] = 1.0f;

    clearValue[1].color.float32[0] = 1.0f;
    clearValue[1].color.float32[2] = 0.0f;

    vk::RenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.renderPass = self->mRenderPass;
    renderPassBeginInfo.framebuffer = self->mFrameBuffers[nextImageIdx];
    renderPassBeginInfo.renderArea = { 0, 0, self->mWidth, self->mHeight };
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValue;

    self->mDrawCommandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    // bind the graphics pipeline to the command buffer. Any vkDraw command afterwards is affeted by this pipeline!
    self->mDrawCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, self->mPipeline);

    // take care of dynamic state:
    vk::Viewport viewport { 0, 0, static_cast<float>(self->mWidth), static_cast<float>(self->mHeight), 0, 1 };
    self->mDrawCommandBuffer.setViewport(0, viewport);

    vk::Rect2D scissor{ {0, 0}, { self->mWidth, self->mHeight } };
    self->mDrawCommandBuffer.setScissor(0, scissor);

    // render the triangle:
    vk::DeviceSize offsets = {};
    self->mDrawCommandBuffer.bindVertexBuffers(0, self->mVertexInputBuffer, offsets);
    self->mDrawCommandBuffer.draw(3, 1, 0, 0);
    self->mDrawCommandBuffer.endRenderPass();

    // change layout back to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    vk::ImageMemoryBarrier prePresentBarrier = {};
    prePresentBarrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    prePresentBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
    prePresentBarrier.oldLayout = vk::ImageLayout::eColorAttachmentOptimal;
    prePresentBarrier.newLayout = vk::ImageLayout::ePresentSrcKHR;
    prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    prePresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    prePresentBarrier.image = self->mPresentImages[nextImageIdx];

    self->mDrawCommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands,
                                             vk::PipelineStageFlagBits::eBottomOfPipe,
                                             vk::DependencyFlags(),
                                             nullptr,
                                             nullptr,
                                             prePresentBarrier);
    self->mDrawCommandBuffer.end();

    // present:
    vk::Fence renderFence = self->mLogicalDevice.createFence(vk::FenceCreateInfo());

    vk::PipelineStageFlags waitStageMash = { vk::PipelineStageFlagBits::eBottomOfPipe };
    vk::SubmitInfo submitInfo = {};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &presentCompleteSemaphore;
    submitInfo.pWaitDstStageMask = &waitStageMash;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &self->mDrawCommandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderingCompleteSemaphore;
    
    self->mQueue.submit(submitInfo, renderFence);

    self->mLogicalDevice.waitForFences(renderFence, true, UINT64_MAX);
    self->mLogicalDevice.destroyFence(renderFence);

    vk::PresentInfoKHR presentInfo = {};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderingCompleteSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &self->mSwapChain;
    presentInfo.pImageIndices = &nextImageIdx;

    self->mQueue.presentKHR(presentInfo);

    self->mLogicalDevice.destroySemaphore(presentCompleteSemaphore);
    self->mLogicalDevice.destroySemaphore(renderingCompleteSemaphore);
  }

  void GraphicsSystem::Update(float aDt)
  {
    if (mVulkanSuccess)
    {
      VulkanRender();
    }

    for (auto &window : mEngine->mWindows)
    {
      window->SwapBuffers();
    }
  }
}
