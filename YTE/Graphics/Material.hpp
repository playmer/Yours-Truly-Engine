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
    void SetupDescriptorSet();
    void UpdateDescriptorSet(Texture &aTexture);

    VulkanContext *mVulkanContext;
    ShaderDescriptions mDescription;
    Shader mVertex;
    Shader mFragment;


    vk::DescriptorSetLayout mDescriptorSetLayout;

    vk::PipelineLayout mPipelineLayout;
    vk::Pipeline mPipeline;
  };
}