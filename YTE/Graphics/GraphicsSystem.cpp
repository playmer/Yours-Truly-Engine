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

  GraphicsSystem::GraphicsSystem(Engine *aEngine) : mEngine(aEngine)
  {
    mVulkanSuccess = vkelInit();
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
    if (mVulkanSuccess)
    {
      auto self = mPlatformSpecificData.ConstructAndGet<vulkan_context>();

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
        std::cout << "Found the Standard Validator. \n";
      }

      vulkan_assert(foundValidator, "Could not find validation layer.");
      const char *enabledLayers[] = { validationLayer };

      instanceInfo.setEnabledLayerCount(1);
      instanceInfo.setPpEnabledLayerNames(enabledLayers);

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

      vulkan_assert(requiredExtensions.size() == foundExtensions, "Could not find debug extension");

      std::array<const char *, 3> requiredExtensions2 = {
        "VK_KHR_surface",
        "VK_KHR_win32_surface",
        "VK_EXT_debug_report"
      };

      instanceInfo.setEnabledExtensionCount((u32)requiredExtensions2.size());
      instanceInfo.setPpEnabledExtensionNames(requiredExtensions2.data());

      auto result = vk::createInstance(&instanceInfo, NULL, &self->mInstance);
      checkVulkanResult(result, "Failed to create vulkan instance.");

      vkelInstanceInit(self->mInstance);


      auto  callbackCreateInfo = vk::DebugReportCallbackCreateInfoEXT()
                                      .setFlags(vk::DebugReportFlagBitsEXT::eError |
                                                vk::DebugReportFlagBitsEXT::eWarning |
                                                vk::DebugReportFlagBitsEXT::ePerformanceWarning)
                                      .setPfnCallback(&DebugReportCallback);

      auto debugReport = self->mInstance.createDebugReportCallbackEXT(callbackCreateInfo);
      vulkan_assert(static_cast<bool>(debugReport), "Failed to create degub report callback.");

      auto windowData = mEngine->mPrimaryWindow->mPlatformSpecificData.Get<WindowData>();
      auto surfaceCreateInfo = vk::Win32SurfaceCreateInfoKHR()
                                    .setHinstance(windowData->mInstance)
                                    .setHwnd(windowData->mWindowHandle);

      self->mSurface = self->mInstance.createWin32SurfaceKHR(surfaceCreateInfo);
    }
  }

  void GraphicsSystem::Update(float aDt)
  {
    if (mVulkanSuccess)
    {
    }

    for (auto &window : mEngine->mWindows)
    {
      window->SwapBuffers();
    }
  }
}
