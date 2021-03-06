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
  class ShaderDescriptions
  {
  public:
    ShaderDescriptions(size_t aNumberOfBindings = 2, size_t aNumberOfAttributes = 8)
    {
      mBindings.reserve(aNumberOfBindings);
      mAttributes.reserve(aNumberOfAttributes);
    }

    template <typename T>
    void AddAttribute(vk::Format aFormat)
    {
      runtime_assert(mBindings.size() != 0, 
                     "Haven't added a Vertex binding yet, so we can't add attribute inputs.");

      vk::VertexInputAttributeDescription toAdd;
      toAdd.binding = mBinding - 1;
      toAdd.location = mLocation;
      toAdd.format = aFormat; // TODO: Do we need the alpha?
      toAdd.offset = mVertexOffset;

      mAttributes.emplace_back(toAdd);

      ++mLocation;
      mVertexOffset += sizeof(T);
    }

    template <typename T>
    void AddBinding(vk::VertexInputRate aDescription)
    {
      vk::VertexInputBindingDescription toAdd;
      toAdd.binding = mBinding;
      toAdd.inputRate = aDescription;
      toAdd.stride = sizeof(T);

      mBindings.emplace_back(toAdd);

      ++mBinding;
      mVertexOffset = 0;
    }

    vk::VertexInputBindingDescription* BindingData() { return mBindings.data(); }
    size_t BindingSize() { return mBindings.size(); }

    vk::VertexInputAttributeDescription* AttributeData() { return mAttributes.data(); }
    size_t AttributeSize() { return mAttributes.size(); }

  private:
    std::vector<vk::VertexInputBindingDescription> mBindings;
    std::vector<vk::VertexInputAttributeDescription> mAttributes;
    u32 mBinding = 0;
    u32 mVertexOffset = 0;
    u32 mLocation = 0;
  };
}

#endif
