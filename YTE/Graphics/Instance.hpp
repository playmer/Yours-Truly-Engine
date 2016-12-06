#pragma once

#include "glm.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"

namespace YTE
{
  // Data to be processed into InstanceDatas
  class Instance
  {
  public:
    Instance(GraphicsSystem *aGraphicsSystem) : mGraphicsSystem(aGraphicsSystem) {}

    inline void SetTranslation(glm::vec3 aTranslation)
    {
      SetChanged(true);
      mTranslation = aTranslation;
    }

    inline void SetScale(glm::vec3 aScale)
    {
      SetChanged(true);
      mScale = aScale;
    }

    inline void SetRotation(glm::vec3 aRotation)
    {
      SetChanged(true);
      mRotation = aRotation;
    }

    inline void SetColor(glm::vec3 aColor)
    {
      SetChanged(true);
      mColor = aColor;
    }

    //inline void SetTranslation(glm::vec3 &aTranslation)
    //{
    //  SetChanged(true);
    //  mTranslation = aTranslation;
    //}
    //
    //inline void SetScale(glm::vec3 &aScale)
    //{
    //  SetChanged(true);
    //  mScale = aScale;
    //}
    //
    //inline void SetRotation(glm::vec3 &aRotation)
    //{
    //  SetChanged(true);
    //  mRotation = aRotation;
    //}
    //
    //inline void SetColor(glm::vec3 &aColor)
    //{
    //  SetChanged(true);
    //  mColor = aColor;
    //}

    inline void SetTextureId(u32 aTextureId)
    {
      SetChanged(true);
      mTextureId = aTextureId;
    }

    void SetChanged(bool aChanged);

  private:
    glm::vec3 mTranslation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 mScale = { 1.0f, 1.0f, 1.0f };
    glm::vec3 mRotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 mColor = { -1.0f, -1.0f, -1.0f };
    u32 mTextureId = 0;
    GraphicsSystem *mGraphicsSystem = nullptr;
    bool mChanged;
  };

  // Data sent to the GPU
  struct InstanceData
  {
    glm::vec4 mRow1 = { 1.0f, 0.0f, 0.0f, 0.0f };
    glm::vec4 mRow2 = { 0.0f, 1.0f, 0.0f, 0.0f };
    glm::vec4 mRow3 = { 0.0f, 0.0f, 1.0f, 0.0f };
    glm::vec4 mRow4 = { 0.0f, 0.0f, 0.0f, 1.0f };
    glm::vec3 mColor = { -1.0f, -1.0f, -1.0f };
    u32 mTextureId = 0;
  };
}