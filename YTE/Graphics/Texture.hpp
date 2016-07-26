#pragma once

#include "vulkan/vkel.h"
#include "vulkan/vk_cpp.hpp"

#include "YTE/Core/Types.hpp"

namespace YTE
{
  struct Texture
  {
    vk::Sampler mSampler;
    vk::Image mImage;
    vk::ImageLayout mImageLayout;
    vk::DeviceMemory mDeviceMemory;
    vk::ImageView mView;
    u32 mWidth;
    u32 mHeight;
    u32 mMipLevels;
    u32 mLayerCount;
    vk::DescriptorImageInfo mDescriptor;
  };
}
