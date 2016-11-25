//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef YTE_ShaderAttributeDescription_hpp
#define YTE_ShaderAttributeDescription_hpp

#include <stddef.h>

#include <vector>

#include "vulkan/vkel.h"
#include "vulkan/vk_cpp.hpp"

#include "YTE/Core/Types.hpp"

namespace YTE
{
  class ShaderAttributeDescriptions
  {
  public:
    ShaderAttributeDescriptions(size_t aSize = 8)
    {
      mDescription.reserve(aSize);
    }

    template <typename T>
    void AddInput(vk::Format aFormat)
    {
      vk::VertexInputAttributeDescription toAdd;
      toAdd.binding = mBinding;
      toAdd.location = mLocation;
      toAdd.format = aFormat; // TODO: Do we need the alpha?
      toAdd.offset = mVertexOffset;

      mDescription.emplace_back(toAdd);

      ++mLocation;
      mVertexOffset += sizeof(T);
    }

    void AddAdditionalVertexBuffer()
    {
      ++mBinding;
      mVertexOffset = 0;
    }

    vk::VertexInputAttributeDescription* data() { return mDescription.data(); }
    vk::VertexInputAttributeDescription* Data() { return mDescription.data(); }
    size_t size() { return mDescription.size(); }
    size_t Size() { return mDescription.size(); }

  private:
    std::vector<vk::VertexInputAttributeDescription> mDescription;
    u32 mBinding = 0;
    u32 mVertexOffset = 0;
    u32 mLocation = 0;
  };
}

#endif
