#include "YTE/Graphics/Material.hpp"
#include "YTE/Graphics/VulkanContext.hpp"


namespace YTE
{

  YTE::Material::Material(VulkanContext *aVulkanContext,
                          ShaderDescriptions aDescription,
                          std::string &aVertexShader,
                          std::string & aFragmentShader)
    : mVulkanContext(aVulkanContext),
      mDescription(aDescription),
      mVertex(aVertexShader, ShaderType::Vertex, aVulkanContext),
      mFragment(aFragmentShader, ShaderType::Vertex, aVulkanContext)
  {
    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStageCreateInfo;
    shaderStageCreateInfo[0] = mVertex.CreateShaderStage();
    shaderStageCreateInfo[1] = mFragment.CreateShaderStage();

    vk::SpecializationInfo info;
    vk::SpecializationMapEntry entry;
    entry.constantID = 0;
    const uint32_t data = static_cast<u32>(aVulkanContext->mTextures.size());
    entry.size = sizeof(u32);

    info.pMapEntries = &entry;
    info.mapEntryCount = 1;
    info.pData = &data;
    info.dataSize = sizeof(u32);

    shaderStageCreateInfo[1].pSpecializationInfo = &info;

    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<u32>(aDescription.BindingSize());
    vertexInputStateCreateInfo.pVertexBindingDescriptions = aDescription.BindingData();
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<u32>(aDescription.AttributeSize());
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = aDescription.AttributeData();

    // vertex topology config:
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
    inputAssemblyStateCreateInfo.topology = vk::PrimitiveTopology::eTriangleList;

    vk::Viewport viewport = {};
    viewport.width = static_cast<float>(aVulkanContext->mView.mWidth);
    viewport.height = static_cast<float>(aVulkanContext->mView.mHeight);
    viewport.maxDepth = 1;

    vk::Rect2D scissors = {};
    scissors.extent.setWidth(aVulkanContext->mView.mWidth)
                   .setHeight(aVulkanContext->mView.mHeight);

    vk::PipelineViewportStateCreateInfo viewportState = {};
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissors;

    vk::PipelineRasterizationStateCreateInfo rasterizationState;
    rasterizationState.polygonMode = vk::PolygonMode::eFill;
    //rasterizationState.cullMode = vk::CullModeFlagBits::eBack; // TODO: Investigate
    rasterizationState.cullMode = vk::CullModeFlagBits::eNone;
    rasterizationState.frontFace = vk::FrontFace::eCounterClockwise;
    //rasterizationState.frontFace = vk::FrontFace::eClockwise;
    rasterizationState.lineWidth = 1;
    rasterizationState.depthClampEnable = true;
    rasterizationState.depthBiasEnable = true;

    vk::PipelineMultisampleStateCreateInfo multisampleState = {};
    multisampleState.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::StencilOpState stencilState = {};
    stencilState.failOp = vk::StencilOp::eReplace;
    stencilState.passOp = vk::StencilOp::eKeep;
    stencilState.depthFailOp = vk::StencilOp::eReplace;
    stencilState.compareOp = vk::CompareOp::eLessOrEqual;

    vk::PipelineDepthStencilStateCreateInfo depthState;
    depthState.depthTestEnable = true;
    depthState.depthWriteEnable = true;
    depthState.depthCompareOp = vk::CompareOp::eLessOrEqual;
    depthState.setDepthBoundsTestEnable(false);
    depthState.setStencilTestEnable(false);

    vk::PipelineColorBlendAttachmentState colorBlendAttachmentState;
    colorBlendAttachmentState.srcColorBlendFactor = vk::BlendFactor::eSrc1Color;
    colorBlendAttachmentState.dstColorBlendFactor = vk::BlendFactor::eOneMinusDstColor;
    colorBlendAttachmentState.colorBlendOp = vk::BlendOp::eAdd;
    colorBlendAttachmentState.srcAlphaBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachmentState.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachmentState.alphaBlendOp = vk::BlendOp::eAdd;
    colorBlendAttachmentState.colorWriteMask = vk::ColorComponentFlagBits::eR |
                                              vk::ColorComponentFlagBits::eG |
                                              vk::ColorComponentFlagBits::eB |
                                              vk::ColorComponentFlagBits::eA;

    vk::PipelineColorBlendStateCreateInfo colorBlendState;
    colorBlendState.logicOpEnable = VK_FALSE;
    colorBlendState.logicOp = vk::LogicOp::eClear;
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &colorBlendAttachmentState;
    colorBlendState.blendConstants[0] = 0.0;
    colorBlendState.blendConstants[1] = 0.0;
    colorBlendState.blendConstants[2] = 0.0;
    colorBlendState.blendConstants[3] = 0.0;

    vk::DynamicState dynamicState[2] = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo;
    dynamicStateCreateInfo.dynamicStateCount = 2;
    dynamicStateCreateInfo.pDynamicStates = dynamicState;

    vk::GraphicsPipelineCreateInfo pipelineCreateInfo;
    pipelineCreateInfo.stageCount = static_cast<u32>(shaderStageCreateInfo.size());
    pipelineCreateInfo.pStages = shaderStageCreateInfo.data();
    pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pDepthStencilState = &depthState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    pipelineCreateInfo.layout = mPipelineLayout;
    pipelineCreateInfo.renderPass = aVulkanContext->mRenderPass;

    aVulkanContext->SetupDescriptorPool();
    aVulkanContext->SetupDescriptorSet();

    mPipeline = aVulkanContext->mLogicalDevice.createGraphicsPipelines(VK_NULL_HANDLE, pipelineCreateInfo)[0];
    vulkan_assert(mPipeline, "Failed to create graphics pipeline.");
  }



  void Material::SetupDescriptorSetLayout()
  {
    // Setup layout of descriptors used in this example
    // Basically connects the different shader stages to descriptors
    // for binding uniform buffers, image samplers, etc.
    // So every shader binding should map to one descriptor set layout
    // binding
    std::array<vk::DescriptorSetLayoutBinding, 2> layoutBinding;

    layoutBinding[0].descriptorCount = 1;
    layoutBinding[0].binding = 0;
    layoutBinding[0].stageFlags = vk::ShaderStageFlagBits::eVertex;
    layoutBinding[0].descriptorType = vk::DescriptorType::eUniformBuffer;

    layoutBinding[1].descriptorCount = static_cast<u32>(mVulkanContext->mTextures.size());
    layoutBinding[1].binding = 1;
    layoutBinding[1].stageFlags = vk::ShaderStageFlagBits::eFragment;
    layoutBinding[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;

    vk::DescriptorType::eStorageBufferDynamic;

    vk::DescriptorSetLayoutCreateInfo descriptorLayout = {};
    descriptorLayout.pNext = nullptr;
    descriptorLayout.bindingCount = static_cast<u32>(layoutBinding.size());
    descriptorLayout.pBindings = layoutBinding.data();

    mDescriptorSetLayout = mVulkanContext->mLogicalDevice.createDescriptorSetLayout(descriptorLayout);

    // Create the pipeline layout that is used to generate the rendering pipelines that
    // are based on this descriptor set layout
    // In a more complex scenario you would have different pipeline layouts for different
    // descriptor set layouts that could be reused
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &mDescriptorSetLayout;

    mPipelineLayout = mVulkanContext->mLogicalDevice.createPipelineLayout(pipelineLayoutCreateInfo);
  }


  void Material::BindPipeline(vk::CommandBuffer &aCommandBuffer)
  {
    // Bind descriptor sets describing shader binding points
    aCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, 
                                      mPipelineLayout, 
                                      0, 
                                      mVulkanContext->mDescriptorSets[mDescriptorSetOffset], 
                                      nullptr);

    // bind the graphics pipeline to the command buffer. Any vkDraw command afterwards is affected by this pipeline!
    aCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);
  }
}
