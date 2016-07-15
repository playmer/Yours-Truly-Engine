//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef YTE_GraphicsSystem_hpp
#define YTE_GraphicsSystem_hpp

#include <stddef.h>

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>

#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Types.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

#include "YTE/Core/PrivateImplementation.hpp"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

namespace YTE
{
  struct Vertex
  {
    void Translate(const glm::vec3 &aTranslation)
    {
      auto translate = glm::translate(glm::mat4(), aTranslation);

      mPosition = translate * mPosition;
    }

    void Scale(const glm::vec3 &aScale)
    {
      auto scale = glm::scale(glm::mat4(), aScale);

      mPosition = scale *mPosition;
    }
    void Rotate(const glm::vec3 &aRotation)
    {
      auto rotation = glm::mat4();
      rotation = glm::rotate(rotation, aRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
      rotation = glm::rotate(rotation, aRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
      rotation = glm::rotate(rotation, aRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

      mPosition = rotation * mPosition;
    }

    glm::vec4 mPosition;
    glm::vec4 mColor;
  };

  struct Triangle
  {
    void Translate(const glm::vec3 &aTranslation)
    {
      mVertex1.Translate(aTranslation);
      mVertex2.Translate(aTranslation);
      mVertex3.Translate(aTranslation);
    }

    void Scale(const glm::vec3 &aScale)
    {
      mVertex1.Scale(aScale);
      mVertex2.Scale(aScale);
      mVertex3.Scale(aScale);
    }
    void Rotate(const glm::vec3 &aRotation)
    {
      mVertex1.Rotate(aRotation);
      mVertex2.Rotate(aRotation);
      mVertex3.Rotate(aRotation);
    }

    Vertex mVertex1;
    Vertex mVertex2;
    Vertex mVertex3;
  };

  struct Quad
  {
    void Translate(const glm::vec3 &aTranslation)
    {
      mTriangle1.Translate(aTranslation);
      mTriangle2.Translate(aTranslation);
    }

    void Scale(const glm::vec3 &aScale)
    {
      mTriangle1.Scale(aScale);
      mTriangle2.Scale(aScale);
    }
    void Rotate(const glm::vec3 &aRotation)
    {
      mTriangle1.Rotate(aRotation);
      mTriangle2.Rotate(aRotation);
    }

    Triangle mTriangle1;
    Triangle mTriangle2;
  };

  class GraphicsSystem
  {
    public:
    GraphicsSystem(Engine *aEngine);
    ~GraphicsSystem();

    void SetUpWindow(Window *aWindow);

    void Initialize();

    void Update(float aDt);
    void VulkanRender();
    void UpdateUniformBuffers();
    void SetupDescriptorSetLayout();
    void SetupDescriptorSet();
    void SetupDescriptorPool();

    Quad *mQuad;
    glm::i16vec2 mMousePosition = { 0,0 };

    private:
    Engine *mEngine;

    PrivateImplementation<2048> mPlatformSpecificData;

	  i32 mVulkanSuccess;
  };

}
#endif
