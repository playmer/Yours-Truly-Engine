//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef YTE_Shader_hpp
#define YTE_Shader_hpp

#include <stddef.h>

#include "YTE/Core/Types.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/VulkanPrimitives.hpp"


namespace YTE
{
  enum class ShaderType
  {
    Vertex = (int)vk::ShaderStageFlagBits::eVertex,
    Fragment = (int)vk::ShaderStageFlagBits::eFragment,
    Geometry = (int)vk::ShaderStageFlagBits::eGeometry,
    Compute = (int)vk::ShaderStageFlagBits::eCompute,
    Unknown
  };

  class Shader
  {
  public:
    Shader(std::string &aFileName, ShaderType aType, VulkanRenderer *aContext);

    vk::PipelineShaderStageCreateInfo CreateShaderStage();

  private:
    vk::ShaderModule mModule;
    ShaderType mType;
  };
}
#endif
