#pragma once

#include "YTE/Core/Types.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/VulkanPrimitives.hpp"

namespace YTE
{
  struct UniformBufferObject
  {
    glm::mat4 mProjectionMatrix;
    glm::mat4 mModelMatrix;
    glm::vec3 mViewPosition;
    float mLevelOfDetailBias = 1.0f;
  };

  class View
  {
  public: 
    View(VulkanContext *aContext);
    ~View();

    void Initialize();
    void UpdateUniformBuffers(bool aRecalculation);


    UniformBufferObject mUniformBufferData;
    BufferMemory mUniform;
    vk::DescriptorBufferInfo mUniformBufferInfo;

    VulkanContext *mContext;

    u32 mWidth = 0;
    u32 mHeight = 0;

    glm::vec3 mRotation = glm::vec3();
    glm::vec3 mCameraPosition = glm::vec3();
  };
}
