#pragma once

#include "vulkan/vkel.h"
#include "vulkan/vk_cpp.hpp"

#include "YTE/Core/Types.hpp"

namespace YTE
{
  struct Texture
  {
    vk::Sampler sampler;
    vk::Image image;
    vk::ImageLayout imageLayout;
    vk::DeviceMemory deviceMemory;
    vk::ImageView view;
    u32 width;
    u32 height;
    u32 mipLevels;
    u32 layerCount;
    vk::DescriptorImageInfo descriptor;
  };
}
