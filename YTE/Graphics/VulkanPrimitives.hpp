#pragma once

#include <set>

#include "YTE/Core/Delegate.hpp"
#include "YTE/Core/Types.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"

#include "vulkan/vkel.h"
#include "vulkan/vulkan.hpp"


namespace YTE
{

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

struct BufferMemory
{
  BufferMemory() {}


  BufferMemory& operator=(BufferMemory &aBufferMemory) = delete;
  BufferMemory(BufferMemory &aBufferMemory) = delete;

  inline BufferMemory(BufferMemory &&aBufferMemory)
    : mMemory(aBufferMemory.mMemory),
    mBuffer(aBufferMemory.mBuffer),
    mLogicalDevice(aBufferMemory.mLogicalDevice)
  {
    aBufferMemory.mLogicalDevice = nullptr;
  }

  inline BufferMemory& operator=(BufferMemory &&aBufferMemory)
  {
    mMemory = aBufferMemory.mMemory;
    mBuffer = aBufferMemory.mBuffer;
    mLogicalDevice = aBufferMemory.mLogicalDevice;
    aBufferMemory.mLogicalDevice = nullptr;

    return *this;
  }

  inline explicit BufferMemory(vk::Device *aLogicalDevice)
    : mLogicalDevice(aLogicalDevice) {}

  ~BufferMemory()
  {
    Destruct();
  }

  inline void Destruct()
  {
    if (nullptr == mLogicalDevice)
    {
      return;
    }

    mLogicalDevice->freeMemory(mMemory);
    mLogicalDevice->destroyBuffer(mBuffer);
    mLogicalDevice = nullptr;
  }

  vk::DeviceMemory mMemory;
  vk::Buffer mBuffer;
  vk::Device *mLogicalDevice = nullptr;
};

class QueueFamilyIndices
{
public:

  static QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice aDevice);
  static void AddRequiredExtension(const char *aExtension);
  static void ClearRequiredExtensions();

  bool IsComplete();
  bool IsDeviceSuitable(vk::PhysicalDevice aDevice, vk::SurfaceKHR aSurface);
  bool CheckDeviceExtensionSupport(vk::PhysicalDevice aDevice);

private:
  static std::vector<const char*> sDeviceExtensions;

  i32 mGraphicsFamily = -1;
};


class SwapChainSupportDetails
{
public:
  static SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice aDevice,
                                                       vk::SurfaceKHR aSurface)
  {
    SwapChainSupportDetails details;

    details.mCapabilities = aDevice.getSurfaceCapabilitiesKHR(aSurface);
    details.mFormats = aDevice.getSurfaceFormatsKHR(aSurface);
    details.mPresentModes = aDevice.getSurfacePresentModesKHR(aSurface);

    return details;
  }

  vk::SurfaceCapabilitiesKHR& Capabilities()
  {
    return mCapabilities;
  }

  std::vector<vk::SurfaceFormatKHR>& Formats()
  {
    return mFormats;
  }

  std::vector<vk::PresentModeKHR>& PresentModes()
  {
    return mPresentModes;
  }

private:
  vk::SurfaceCapabilitiesKHR mCapabilities;
  std::vector<vk::SurfaceFormatKHR> mFormats;
  std::vector<vk::PresentModeKHR> mPresentModes;
};

template <typename T>
class VulkanDeleter
{
  VulkanDeleter()
  {

  }

  Delegate<void(*)(T*)> mDeleter;
  T mData;
};


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


}