#include <iostream>

#include "Windows.h"

#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"

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

	if (false == mVulkanSuccess)
	{
		return;
	}

    auto self = mPlatformSpecificData.ConstructAndGet<vulkan_context>();

    auto appInfo = vk::ApplicationInfo()
                     .setPApplicationName("First Test")
                     .setEngineVersion(1)
                     .setApiVersion(VK_MAKE_VERSION(1,0,0));

    auto instanceInfo = vk::InstanceCreateInfo()
                        .setPApplicationInfo(&appInfo);

    auto result = vk::createInstance(&instanceInfo, NULL, &self->mInstance);
    checkVulkanResult(result, "Failed to create vulkan instance.");

    auto layers = vk::enumerateInstanceLayerProperties();
    vulkan_assert(layers.size(), "Failed to find layers.");

    bool foundValidator = false;

    for (auto &layer : layers)
    {
      std::cout << "Name: " << layer.layerName << "\n  Description: " << layer.description << "\n";

      if (std::strcmp(layer.layerName, "VK_LAYER_LUNARG_standard_validation") == 0)
      {
        foundValidator = true;
      }
    }

    if (foundValidator)
    {
      std::cout << "Found the Standard Validator. \n";
    }

    vulkan_assert(foundValidator, "Could not find validation layer.");
    const char *enabledLayers[] = { "VK_LAYER_LUNARG_standard_validation" };
    
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

    auto  callbackCreateInfo = vk::DebugReportCallbackCreateInfoEXT()
                                   .setFlags(vk::DebugReportFlagBitsEXT::eError | 
                                             vk::DebugReportFlagBitsEXT::eWarning | 
                                             vk::DebugReportFlagBitsEXT::ePerformanceWarning)
                                   .setPfnCallback(&DebugReportCallback);

    vkelInstanceInit(self->mInstance);

    auto debugReport = self->mInstance.createDebugReportCallbackEXT(callbackCreateInfo);
    vulkan_assert(static_cast<bool>(debugReport), "Failed to create degub report callback.");
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
