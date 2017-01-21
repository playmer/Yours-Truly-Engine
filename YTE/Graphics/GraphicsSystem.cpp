#include <iostream>
#include <filesystem>
#include <string>

#include "vulkan/vkel.h"
#include "vulkan/vk_cpp.hpp"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

//TODO: Fix this
#define DIRECTINPUT_VERSION 0x0800
#define WIN32_LEAN_AND_MEAN 1
#define VC_EXTRALEAN
#include "Windows.h"

#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/Model.hpp"
#include "YTE/Graphics/Shader.hpp"
#include "YTE/Graphics/TextureLoader.hpp"
#include "YTE/Graphics/ShaderDescriptions.hpp"
#include "YTE/Graphics/VulkanContext.hpp"

#include "YTE/Platform/Window.hpp"
#include "YTE/Platform/Windows/WindowData_Windows.hpp"

namespace YTE
{
  enum class StringComparison
  {
    String1Null,     // (We check this first)
    LesserInString1, // The first character that does not match has a lower value in ptr1 than in ptr2
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
    printf("%s: %s\n", aLayerPrefix, aMessage);
    return VK_FALSE;
  }

  GraphicsSystem::GraphicsSystem(Engine *aEngine) 
    : mEngine(aEngine), mVulkanSuccess(0)
  {
    auto self = mPlatformSpecificData.ConstructAndGet<VulkanContext>();

    #define YTERegisterEvent(EventType, ObjectInstance, MemberFunctionPtr) \
      RegisterEvent<decltype(MemberFunctionPtr), MemberFunctionPtr>(EventType, ObjectInstance)

    static const std::string update = "LogicUpdate";
    mEngine->YTERegisterEvent(update, this, &GraphicsSystem::Update);
  }

  GraphicsSystem::~GraphicsSystem()
  {
    if (mVulkanSuccess)
    {
      mMeshes.clear();

      mPlatformSpecificData.Destruct();
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

    mBaseQuad.mVertex1.mPosition = { -0.5f, -0.5f, 0.0f, 1.0f };
    mBaseQuad.mVertex1.mUVCoordinates = { 0.0f, 0.0f };
    mBaseQuad.mVertex1.mNormal = { 0.0f, 0.0f, 1.0 };

    mBaseQuad.mVertex2.mPosition = { 0.5f, -0.5f, 0.0f, 1.0f };
    mBaseQuad.mVertex2.mUVCoordinates = { 1.0f, 0.0f };
    mBaseQuad.mVertex2.mNormal = { 0.0f, 0.0f, 1.0 };

    mBaseQuad.mVertex3.mPosition = { 0.5f, 0.5f, 0.0f, 1.0f };
    mBaseQuad.mVertex3.mUVCoordinates = { 1.0f, 1.0f };
    mBaseQuad.mVertex3.mNormal = { 0.0f, 0.0f, 1.0 };

    mBaseQuad.mVertex4.mPosition = { -0.5f, 0.5f, 0.0f, 1.0f };
    mBaseQuad.mVertex4.mUVCoordinates = { 0.0f, 1.0f };
    mBaseQuad.mVertex4.mNormal = { 0.0f, 0.0f, 1.0 };

    if (mVulkanSuccess)
    {
      auto self = mPlatformSpecificData.Get<VulkanContext>();

      vk::ApplicationInfo appInfo;
      appInfo.setPApplicationName("First Test");
      appInfo.setEngineVersion(1);
      appInfo.setApiVersion(VK_MAKE_VERSION(1, 0, 0));

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
        #if _DEBUG
        instanceInfo.setEnabledLayerCount(1);
        instanceInfo.setPpEnabledLayerNames(enabledLayers);
        #endif
      }
      else
      {
          printf("Could not find validation layers.");
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

        auto result = vk::createInstance(&instanceInfo, nullptr, &self->mInstance);
        checkVulkanResult(result, "Failed to create vulkan instance.");
      }
      else
      {
        printf("Could not find debug extension");
      }

      vkelInstanceInit(self->mInstance);

      if (requiredExtensions.size() == foundExtensions)
      {
        vk::DebugReportCallbackCreateInfoEXT callbackCreateInfo;
        callbackCreateInfo.setFlags(vk::DebugReportFlagBitsEXT::eError |
                                    vk::DebugReportFlagBitsEXT::eWarning |
                                    vk::DebugReportFlagBitsEXT::ePerformanceWarning);
        callbackCreateInfo.setPfnCallback(&DebugReportCallback);

        auto debugReport = self->mInstance.createDebugReportCallbackEXT(callbackCreateInfo);
        vulkan_assert(static_cast<bool>(debugReport), "Failed to create degub report callback.");
      }

      // TODO: Abstract this for multiple windows.
      auto window = mEngine->mPrimaryWindow;
      auto windowData = window->mPlatformSpecificData.Get<WindowData>();
      vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo;
      surfaceCreateInfo.setHinstance(windowData->mInstance);
      surfaceCreateInfo.setHwnd(windowData->mWindowHandle);

      self->mView.mHeight = window->mHeight;
      self->mView.mWidth = window->mWidth;

      self->mSurface = self->mInstance.createWin32SurfaceKHR(surfaceCreateInfo);

      auto physicalDevices = self->mInstance.enumeratePhysicalDevices();

      u32 deviceCount = 0;
      for (auto &physicalDevice : physicalDevices)
      {
        auto properties = physicalDevice.getProperties();

        auto queueProperties = physicalDevice.getQueueFamilyProperties();

        printf("Device #%d: %s\n", deviceCount, properties.deviceName);

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
          ++deviceCount;
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
      vk::DeviceQueueCreateInfo queueCreateInfo;
      queueCreateInfo.setQueueFamilyIndex(self->mPresentQueueIdx);
      queueCreateInfo.setQueueCount(1);

      float queuePriorities[] = { 1.0f };   // ask for highest priority for our queue. (range [0,1])
      queueCreateInfo.pQueuePriorities = queuePriorities;

      const char *deviceExtensions[] = { "VK_KHR_swapchain" };

      vk::DeviceCreateInfo deviceInfo;
      deviceInfo.setQueueCreateInfoCount(1);
      deviceInfo.setPQueueCreateInfos(&queueCreateInfo);
      deviceInfo.setEnabledLayerCount(1);
      deviceInfo.setPpEnabledLayerNames(enabledLayers);
      deviceInfo.setEnabledExtensionCount(1);
      deviceInfo.setPpEnabledExtensionNames(deviceExtensions);

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
        surfaceResolution.width = self->mView.mWidth;
        surfaceResolution.height = self->mView.mHeight;
      }
      else 
      {
        self->mView.mWidth = surfaceResolution.width;
        self->mView.mHeight = surfaceResolution.height;
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
          // This is what we'd prefer.
        if (presentMode == vk::PresentModeKHR::eMailbox)
        {
          presentationMode = vk::PresentModeKHR::eMailbox;
          break;
        }
          // We'll take this if Mailbox isn't available.
        else if (presentMode == vk::PresentModeKHR::eFifoRelaxed)
        {
          presentationMode = vk::PresentModeKHR::eFifoRelaxed;
        }
      }

      vk::SwapchainCreateInfoKHR swapChainCreateInfo;
      swapChainCreateInfo.setSurface(self->mSurface);
      swapChainCreateInfo.setMinImageCount(desiredImageCount);
      swapChainCreateInfo.setImageFormat(colorFormat);
      swapChainCreateInfo.setImageColorSpace(colorSpace);
      swapChainCreateInfo.setImageExtent(surfaceResolution);
      swapChainCreateInfo.setImageArrayLayers(1);
      swapChainCreateInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
      swapChainCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);   // TODO: Learn how to share queues.
      swapChainCreateInfo.setPreTransform(preTransform);
      swapChainCreateInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque); //TODO: Pretty sure we want to do pre-multiplied.
      swapChainCreateInfo.setPresentMode(presentationMode);
      swapChainCreateInfo.setClipped(true); // If we want clipping outside the extents
                                            // (remember our device features?)

      self->mSwapChain = self->mLogicalDevice.createSwapchainKHR(swapChainCreateInfo);
      vulkan_assert(self->mSwapChain, "Failed to create swapchain.");

      self->mQueue = self->mLogicalDevice.getQueue(self->mPresentQueueIdx, 0);

      vk::CommandPoolCreateInfo commandPoolCreateInfo;
      commandPoolCreateInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
      commandPoolCreateInfo.setQueueFamilyIndex(self->mPresentQueueIdx);

      self->mCommandPool = self->mLogicalDevice.createCommandPool(commandPoolCreateInfo);
      vulkan_assert(self->mCommandPool, "Failed to create command pool.");

      vk::CommandBufferAllocateInfo commandBufferAllocationInfo;
      commandBufferAllocationInfo.setCommandPool(self->mCommandPool);
      commandBufferAllocationInfo.setLevel(vk::CommandBufferLevel::ePrimary);
      commandBufferAllocationInfo.setCommandBufferCount(1);

      self->mSetupCommandBuffer = self->mLogicalDevice.allocateCommandBuffers(commandBufferAllocationInfo)[0];
      vulkan_assert(self->mSetupCommandBuffer, "Failed to allocate setup command buffer.");

      self->mPresentImages = self->mLogicalDevice.getSwapchainImagesKHR(self->mSwapChain);

      commandBufferAllocationInfo.commandBufferCount = static_cast<u32>(self->mPresentImages.size());
      self->mDrawCommandBuffers = self->mLogicalDevice.allocateCommandBuffers(commandBufferAllocationInfo);
      vulkan_assert(self->mDrawCommandBuffers.size(), "Failed to allocate draw command buffer.");

      vk::ImageViewCreateInfo presentImagesViewCreateInfo;
      presentImagesViewCreateInfo.setViewType(vk::ImageViewType::e2D);
      presentImagesViewCreateInfo.setFormat(colorFormat);
      presentImagesViewCreateInfo.setFlags((vk::ImageViewCreateFlagBits)0);
      presentImagesViewCreateInfo.setComponents({ vk::ComponentSwizzle::eR,
                                                  vk::ComponentSwizzle::eG,
                                                  vk::ComponentSwizzle::eB,
                                                  vk::ComponentSwizzle::eA });

      presentImagesViewCreateInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
      presentImagesViewCreateInfo.subresourceRange.setLevelCount(1);
      presentImagesViewCreateInfo.subresourceRange.setLayerCount(1);

      vk::ImageCreateInfo imageCreateInfo;
      imageCreateInfo.setImageType(vk::ImageType::e2D);
      imageCreateInfo.setFormat(vk::Format::eD32SfloatS8Uint);
      imageCreateInfo.setExtent({ self->mView.mWidth, self->mView.mHeight, 1 });
      imageCreateInfo.setMipLevels(1);
      imageCreateInfo.setArrayLayers(1);
      imageCreateInfo.setSamples(vk::SampleCountFlagBits::e1);
      imageCreateInfo.setTiling(vk::ImageTiling::eOptimal);
      imageCreateInfo.setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment);
      imageCreateInfo.setSharingMode(vk::SharingMode::eExclusive); // TODO: Change this when you learn more.
      imageCreateInfo.setInitialLayout(vk::ImageLayout::eUndefined);

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
      
          vk::ImageMemoryBarrier layoutTransitionBarrier;
          layoutTransitionBarrier.setDstAccessMask(vk::AccessFlagBits::eMemoryRead);
          layoutTransitionBarrier.setOldLayout(vk::ImageLayout::eUndefined);
          layoutTransitionBarrier.setNewLayout(vk::ImageLayout::ePresentSrcKHR);
          layoutTransitionBarrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
          layoutTransitionBarrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
          layoutTransitionBarrier.setImage(self->mPresentImages[nextImageIdx]);
      
          vk::ImageSubresourceRange resourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };
          layoutTransitionBarrier.setSubresourceRange(resourceRange);
          vk::ClearColorValue clear;
          clear.setFloat32( { 0.0f, 0.0f, 0.0f, 1.0f });
          self->mSetupCommandBuffer.clearColorImage(self->mPresentImages[nextImageIdx],
                                                    vk::ImageLayout::eTransferDstOptimal,
                                                    clear,
                                                    resourceRange);
      
          self->mSetupCommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, 
                                                    vk::PipelineStageFlagBits::eTopOfPipe, 
                                                    vk::DependencyFlags(),
                                                    nullptr, 
                                                    nullptr,
                                                    layoutTransitionBarrier);
      
          self->mSetupCommandBuffer.end();
      
          vk::PipelineStageFlags waitStageMash[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
          vk::SubmitInfo submitInfo;
          submitInfo.setWaitSemaphoreCount(1);
          submitInfo.setPWaitSemaphores(&presentCompleteSemaphore);
          submitInfo.setPWaitDstStageMask(waitStageMash);
          submitInfo.setCommandBufferCount(1);
          submitInfo.setPCommandBuffers(&self->mSetupCommandBuffer);
      
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

      namespace fs = std::experimental::filesystem;

      TextureLoader loader(self->mPhysicalDevice, self->mLogicalDevice, self->mQueue, self->mCommandPool);

      u32 i = 0;
      for (auto& file : fs::directory_iterator(L"./Textures"))
      {
        std::string texturePath = file.path().string();
        self->mTextures.emplace_back(loader.loadTexture(texturePath));
      }

      self->mSetupCommandBuffer.begin(beginInfo);
      vk::ImageMemoryBarrier layoutTransitionBarrier;
      layoutTransitionBarrier.setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentRead |
                                               vk::AccessFlagBits::eDepthStencilAttachmentWrite);
      layoutTransitionBarrier.setOldLayout(vk::ImageLayout::eUndefined);
      layoutTransitionBarrier.setNewLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
      layoutTransitionBarrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
      layoutTransitionBarrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
      layoutTransitionBarrier.setImage(self->mDepthImage);

      auto resourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, 0, 1, 0, 1);

      layoutTransitionBarrier.setSubresourceRange(resourceRange);

      self->mSetupCommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                                                vk::PipelineStageFlagBits::eTopOfPipe,
                                                vk::DependencyFlags(),
                                                nullptr,
                                                nullptr,
                                                layoutTransitionBarrier);

      self->mSetupCommandBuffer.end();

      vk::PipelineStageFlags waitStageMask[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
      vk::SubmitInfo submitInfo;
      submitInfo.setPWaitDstStageMask(waitStageMask);
      submitInfo.setCommandBufferCount(1);
      submitInfo.setPCommandBuffers(&self->mSetupCommandBuffer);

      self->mQueue.submit(submitInfo, submitFence);

      self->mLogicalDevice.waitForFences(submitFence, true, UINT64_MAX);
      self->mLogicalDevice.resetFences(submitFence);

      // NOTE: Instead of eReleaseResources should it be 0?
      self->mSetupCommandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);

      vk::ImageAspectFlags aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
      vk::ImageViewCreateInfo imageViewCreateInfo;
      imageViewCreateInfo.setImage(self->mDepthImage);
      imageViewCreateInfo.setViewType(vk::ImageViewType::e2D);
      imageViewCreateInfo.setFormat(imageCreateInfo.format);
      imageViewCreateInfo.setComponents({ vk::ComponentSwizzle::eIdentity,
                                          vk::ComponentSwizzle::eIdentity,
                                          vk::ComponentSwizzle::eIdentity ,
                                          vk::ComponentSwizzle::eIdentity });
      imageViewCreateInfo.setSubresourceRange(vk::ImageSubresourceRange(aspectMask, 0, 1, 0, 1));

      self->mDepthImageView = self->mLogicalDevice.createImageView(imageViewCreateInfo);

      vulkan_assert(self->mDepthImageView, "Failed to create image view.");

      std::array<vk::AttachmentDescription, 2> passAttachments = {};
      passAttachments[0].format = colorFormat;
      passAttachments[0].samples = vk::SampleCountFlagBits::e1;
      passAttachments[0].loadOp = vk::AttachmentLoadOp::eClear;
      passAttachments[0].storeOp = vk::AttachmentStoreOp::eStore;
      passAttachments[0].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
      passAttachments[0].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
      passAttachments[0].initialLayout = vk::ImageLayout::eColorAttachmentOptimal;
      passAttachments[0].finalLayout = vk::ImageLayout::eColorAttachmentOptimal;

      //passAttachments[1].format = vk::Format::eD16Unorm;
      passAttachments[1].format = vk::Format::eD32SfloatS8Uint;
      passAttachments[1].samples = vk::SampleCountFlagBits::e1;
      passAttachments[1].loadOp = vk::AttachmentLoadOp::eClear;
      passAttachments[1].storeOp = vk::AttachmentStoreOp::eDontCare;
      passAttachments[1].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
      passAttachments[1].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
      passAttachments[1].initialLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
      passAttachments[1].finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

      vk::AttachmentReference colorAttachmentReference;
      colorAttachmentReference.setAttachment(0); // First attachment is the color attachment.
      colorAttachmentReference.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

      vk::AttachmentReference depthAttachmentReference;
      depthAttachmentReference.setAttachment(1); // Second attachment is the depth attachment.
      depthAttachmentReference.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

      vk::SubpassDescription subpass;
      subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
      subpass.setColorAttachmentCount(1);
      subpass.setPColorAttachments(&colorAttachmentReference);
      subpass.setPDepthStencilAttachment(&depthAttachmentReference);

      vk::SubpassDependency subpassDependency;
      subpassDependency.setSrcStageMask(vk::PipelineStageFlagBits::eTopOfPipe);
      subpassDependency.setDstStageMask(vk::PipelineStageFlagBits::eTopOfPipe);
      //subpassDependency.setSrcAccessMask(vk::AccessFlagBits(0));
      //subpassDependency.setDstAccessMask(vk::AccessFlagBits(0));
      //subpassDependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
      //subpassDependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
      subpassDependency.setSrcSubpass(0);
      subpassDependency.setDstSubpass(0);

      vk::RenderPassCreateInfo renderPassCreateInfo;
      renderPassCreateInfo.setAttachmentCount(static_cast<u32>(passAttachments.size()));
      renderPassCreateInfo.setPAttachments(passAttachments.data());
      renderPassCreateInfo.setSubpassCount(1);
      renderPassCreateInfo.setDependencyCount(1);
      renderPassCreateInfo.setPDependencies(&subpassDependency);
      renderPassCreateInfo.setPSubpasses(&subpass);

      self->mRenderPass = self->mLogicalDevice.createRenderPass(renderPassCreateInfo);
      vulkan_assert(self->mRenderPass, "Failed to create renderpass");
      
      // create our frame buffers:
      vk::ImageView frameBufferAttachments[2];
      frameBufferAttachments[1] = self->mDepthImageView;

      vk::FramebufferCreateInfo frameBufferCreateInfo = {};
      frameBufferCreateInfo.renderPass = self->mRenderPass;
      frameBufferCreateInfo.attachmentCount = 2;  // must be equal to the attachment count on render pass
      frameBufferCreateInfo.pAttachments = frameBufferAttachments;
      frameBufferCreateInfo.width = self->mView.mWidth;
      frameBufferCreateInfo.height = self->mView.mHeight;
      frameBufferCreateInfo.layers = 1;

      // create a framebuffer per swap chain imageView:
      auto imageCount = self->mPresentImages.size();
      self->mFrameBuffers = std::vector<vk::Framebuffer>(imageCount, vk::Framebuffer());
      for (u32 i = 0; i < imageCount; ++i) 
      {
        frameBufferAttachments[0] = self->mPresentImageViews[i];
        auto result = self->mLogicalDevice.createFramebuffer(&frameBufferCreateInfo, NULL, &self->mFrameBuffers[i]);
        checkVulkanResult(result, "Failed to create framebuffer.");
      }

      self->SetupDescriptorSetLayout();


      ShaderDescriptions descriptions;
      descriptions.AddBinding<Vertex>(vk::VertexInputRate::eVertex);

      //glm::vec4 mPosition;
      descriptions.AddAttribute<glm::vec4>(vk::Format::eR32G32B32A32Sfloat);

      //glm::vec2 mUVCoordinates;
      descriptions.AddAttribute<glm::vec2>(vk::Format::eR32G32Sfloat);

      //glm::vec2 mNormal;
      descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

      ///////////////////////////////////////////////////////////
      // Instance Attributes
      ///////////////////////////////////////////////////////////
      descriptions.AddBinding<InstanceData>(vk::VertexInputRate::eInstance);

      //glm::vec3 mTranslation
      descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

      //glm::vec3 mScale
      descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

      //glm::vec3 mRotation;
      descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

      //glm::vec3 mColor
      descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

      //u32 mTextureId
      descriptions.AddAttribute<u32>(vk::Format::eR32Uint);

      //glm::mat4 mTransform;
      descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32A32Sfloat);
      descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32A32Sfloat);
      descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32A32Sfloat);
      descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32A32Sfloat);

      mMaterials.emplace_back(self, descriptions, "./Shaders/vert.spv"s, "./Shaders/frag.spv"s);
      
      YTE::Vertex mVertex1;

      mVertex1.mPosition = { -0.5f, -0.5f, 0.0f, 1.0f };
      mVertex1.mUVCoordinates = { 0.0f, 0.0f };
      mVertex1.mNormal = { 0.0f, 0.0f, 1.0 };

      YTE::Vertex mVertex2;

      mVertex2.mPosition = { 0.5f, -0.5f, 0.0f, 1.0f };
      mVertex2.mUVCoordinates = { 1.0f, 0.0f };
      mVertex2.mNormal = { 0.0f, 0.0f, 1.0 };

      YTE::Vertex mVertex3;

      mVertex3.mPosition = { 0.5f, 0.5f, 0.0f, 1.0f };
      mVertex3.mUVCoordinates = { 1.0f, 1.0f };
      mVertex3.mNormal = { 0.0f, 0.0f, 1.0 };

      YTE::Vertex mVertex4;

      mVertex4.mPosition = { -0.5f, 0.5f, 0.0f, 1.0f };
      mVertex4.mUVCoordinates = { 0.0f, 1.0f };
      mVertex4.mNormal = { 0.0f, 0.0f, 1.0 };

      auto verticies = { mVertex1, mVertex2, mVertex3, mVertex4 };
      auto indicies = { 0u, 1u, 2u, 2u, 3u, 0u };

      mMeshes.emplace_back(verticies, indicies, self);
    } 
  }

  void GraphicsSystem::SetupDrawing()
  {
    auto self = mPlatformSpecificData.Get<VulkanContext>();

    i8 i = 0;
    for (auto &commandBuffer : self->mDrawCommandBuffers)
    {
      vk::CommandBufferBeginInfo beginInfo = {};
      beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

      commandBuffer.begin(beginInfo);

      // change image layout from VK_IMAGE_LAYOUT_PRESENT_SRC_KHR to VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
      vk::ImageMemoryBarrier layoutTransitionBarrier = {};
      layoutTransitionBarrier.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
      layoutTransitionBarrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
        vk::AccessFlagBits::eColorAttachmentWrite;
      layoutTransitionBarrier.oldLayout = vk::ImageLayout::ePresentSrcKHR;
      layoutTransitionBarrier.newLayout = vk::ImageLayout::eColorAttachmentOptimal;
      layoutTransitionBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      layoutTransitionBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      layoutTransitionBarrier.image = self->mPresentImages[i];

      auto resourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
      layoutTransitionBarrier.subresourceRange = resourceRange;

      commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                                    vk::PipelineStageFlagBits::eTopOfPipe,
                                    vk::DependencyFlags(),
                                    nullptr,
                                    nullptr,
                                    layoutTransitionBarrier);

      // activate render pass:
      vk::ClearValue clearValue[2];
      clearValue[0].color.float32[0] = 0.0f;
      clearValue[0].color.float32[1] = 0.0f;
      clearValue[0].color.float32[2] = 0.0f;
      clearValue[0].color.float32[3] = 1.0f;

      //color.float32[0] = 1.0f;
      //color.float32[2] = 0.0f;
      decltype(clearValue[1].depthStencil) stencil{ 1.0f, 0 };
      clearValue[1].depthStencil = stencil;

      vk::RenderPassBeginInfo renderPassBeginInfo = {};
      renderPassBeginInfo.renderPass = self->mRenderPass;
      renderPassBeginInfo.framebuffer = self->mFrameBuffers[i];
      renderPassBeginInfo.renderArea = { 0, 0, self->mView.mWidth, self->mView.mHeight };
      renderPassBeginInfo.clearValueCount = 2;
      renderPassBeginInfo.pClearValues = clearValue;

      vk::Viewport viewport{ 0, 0, static_cast<float>(self->mView.mWidth), static_cast<float>(self->mView.mHeight), 0, 1 };
      vk::Rect2D scissor{ { 0, 0 },{ self->mView.mWidth, self->mView.mHeight } };

      commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

      // Bind descriptor sets describing shader binding points
      commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, self->mPipelineLayout, 0, self->mDescriptorSets, nullptr);

      // bind the graphics pipeline to the command buffer. Any vkDraw command afterwards is affected by this pipeline!
      commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, self->mPipeline);

      // take care of dynamic state:
      commandBuffer.setViewport(0, viewport);
      commandBuffer.setScissor(0, scissor);

      // render the triangle:
      vk::DeviceSize offsets = {};

      commandBuffer.bindVertexBuffers(0, mQuadVerticies.mBuffer, offsets);
      commandBuffer.bindVertexBuffers(1, mInstanceDataBuffer.mBuffer, offsets);
      commandBuffer.bindIndexBuffer(mQuadIndicies.mBuffer, 0, vk::IndexType::eUint32);
      commandBuffer.drawIndexed(6, static_cast<u32>(mInstanceDatas.size()), 0, 0, 0);

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
      prePresentBarrier.image = self->mPresentImages[i];

      commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands,
        vk::PipelineStageFlagBits::eBottomOfPipe,
        vk::DependencyFlags(),
        nullptr,
        nullptr,
        prePresentBarrier);
      commandBuffer.end();

      ++i;
    }
  }

  void GraphicsSystem::VulkanRender()
  {
    auto self = mPlatformSpecificData.Get<VulkanContext>();

    if (mInstanceDataBufferSize < mInstances.size())
    {
      SetupInstanceDataBuffer();

      SetupDrawing();
    }

    auto bufferSize = static_cast<u32>(mInstanceDatas.size() * sizeof(InstanceData));

    //for (auto &object : mObjects)
    //{
    //  glm::mat4 objectToWorld = glm::translate(glm::mat4(), object.mTranslation);
    //  
    //  objectToWorld = glm::rotate(objectToWorld, object.mRotation.x, glm::vec3(1.0, 0.0, 0.0));
    //  objectToWorld = glm::rotate(objectToWorld, object.mRotation.y, glm::vec3(0.0, 1.0, 0.0));
    //  objectToWorld = glm::rotate(objectToWorld, object.mRotation.z, glm::vec3(0.0, 0.0, 1.0));
    //
    //  objectToWorld = glm::scale(objectToWorld, object.mScale);
    //  object.mTransform = objectToWorld;
    //}

    auto objectsBufferPtr = static_cast<InstanceData*>(self->mLogicalDevice.mapMemory(mInstanceDataBuffer.mMemory, 0, bufferSize));
    memcpy(objectsBufferPtr, mInstanceDatas.data(), bufferSize);
    self->mLogicalDevice.unmapMemory(mInstanceDataBuffer.mMemory);

    self->UpdateUniformBuffers(true);
    
    vk::SemaphoreCreateInfo semaphoreCreateInfo = vk::SemaphoreCreateInfo();

    auto presentCompleteSemaphore = self->mLogicalDevice.createSemaphore(semaphoreCreateInfo);
    auto renderingCompleteSemaphore = self->mLogicalDevice.createSemaphore(semaphoreCreateInfo);

    auto result = self->mLogicalDevice.acquireNextImageKHR(self->mSwapChain, 
                                                           UINT64_MAX, 
                                                           presentCompleteSemaphore, 
                                                           VK_NULL_HANDLE, 
                                                           &self->mCurrentDrawBuffer);

    checkVulkanResult(result, "Could not acquireNextImageKHR.");
    
    // present:
    vk::Fence renderFence = self->mLogicalDevice.createFence(vk::FenceCreateInfo());

    vk::PipelineStageFlags waitStageMash = { vk::PipelineStageFlagBits::eBottomOfPipe };
    vk::SubmitInfo submitInfo = {};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &presentCompleteSemaphore;
    submitInfo.pWaitDstStageMask = &waitStageMash;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &self->mDrawCommandBuffers[self->mCurrentDrawBuffer];
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
    presentInfo.pImageIndices = &self->mCurrentDrawBuffer;

    self->mQueue.presentKHR(presentInfo);

    self->mLogicalDevice.destroySemaphore(presentCompleteSemaphore);
    self->mLogicalDevice.destroySemaphore(renderingCompleteSemaphore);
  }

  void GraphicsSystem::SetupInstanceDataBuffer()
  {
    auto self = mPlatformSpecificData.Get<VulkanContext>();

    // We need mInstances amount of InstanceDatas
    auto bytesNeeded = static_cast<u32>(mInstances.size() * sizeof(InstanceData));

    mInstanceDataBuffer = self->CreateBuffer(bytesNeeded, 
                                             vk::BufferUsageFlagBits::eVertexBuffer, 
                                             vk::MemoryPropertyFlagBits::eHostVisible | 
                                             vk::MemoryPropertyFlagBits::eHostCoherent);

    mInstanceDataBufferSize = static_cast<u32>(mInstances.size());
  }

  void GraphicsSystem::Update(LogicUpdate *aUpdate)
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
