#pragma once

#include <string>

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/Shader.hpp"
#include "YTE/Graphics/ShaderDescriptions.hpp"

namespace YTE
{
  class Material
  {
  public:
    Material(VulkanContext *aVulkanContext, 
             ShaderDescriptions aDescription, 
             std::string &aVertexShader, 
             std::string &aFragmentShader);


    void SetupDescriptorSetLayout();

    void BindPipeline(vk::CommandBuffer &aCommandBuffer);

    VulkanContext *mVulkanContext;
    ShaderDescriptions mDescription;
    Shader mVertex;
    Shader mFragment;


    vk::DescriptorSetLayout mDescriptorSetLayout;
    u32 mDescriptorSetOffset = 0;

    vk::PipelineLayout mPipelineLayout;
    vk::Pipeline mPipeline;
  };
}