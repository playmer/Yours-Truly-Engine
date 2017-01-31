#include "VulkanPrimitives.hpp"

namespace YTE
{
  bool QueueFamilyIndices::IsDeviceSuitable(vk::PhysicalDevice aDevice, vk::SurfaceKHR aSurface)
  {
    auto indices = FindQueueFamilies(aDevice);

    bool extensionsSupported = CheckDeviceExtensionSupport(aDevice);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
      SwapChainSupportDetails swapChainSupport = SwapChainSupportDetails::QuerySwapChainSupport(aDevice, aSurface);
      swapChainAdequate = !swapChainSupport.Formats().empty() && !swapChainSupport.PresentModes().empty();
    }

    return indices.IsComplete() && extensionsSupported && swapChainAdequate;
  }
}