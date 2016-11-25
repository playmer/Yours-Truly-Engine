#include <iostream>

#include "YTE/Graphics/Shader.hpp"
#include "YTE/Graphics/VulkanContext.hpp"

namespace YTE
{
  Shader::Shader(const char *aFileName, ShaderType aType, VulkanContext *aContext)
    : mType(aType)
  {
    std::FILE *fp = std::fopen(aFileName, "rb");
    std::fseek(fp, 0, SEEK_END); // seek to end of file
    auto size = std::ftell(fp); // get current file pointer
    std::fseek(fp, 0, SEEK_SET); // seek back to beginning of file

    auto file = (byte*)std::malloc(size);
    std::fread(file, sizeof(byte), size, fp);
    std::fclose(fp);

    vk::ShaderModuleCreateInfo shaderCreationInfo;
    shaderCreationInfo.setCodeSize(size);
    shaderCreationInfo.setPCode((uint32_t *)file);

    mModule = aContext->mLogicalDevice.createShaderModule(shaderCreationInfo);
    vulkan_assert(mModule, "Failed to create shader module.");
    free(file);
  }

  vk::PipelineShaderStageCreateInfo Shader::CreateShaderStage()
  {
    vk::PipelineShaderStageCreateInfo toReturn;
    toReturn.stage = (vk::ShaderStageFlagBits)mType;
    toReturn.module = mModule;
    toReturn.pName = "main";        // shader entry point function name
    return toReturn;
  }
}