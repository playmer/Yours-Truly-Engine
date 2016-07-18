//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef YTE_Vertex_hpp
#define YTE_Vertex_hpp

#include <vector>

#include "glm.hpp"

struct Vertex
{
  glm::vec3 mPosition;
  glm::vec2 mTexture;
  glm::vec3 mNormal;
  glm::vec3 mColor;
  glm::vec3 mTangent;
  glm::vec3 mBinormal;

  Vertex() {}

  Vertex(const glm::vec3& aPosition, const glm::vec2& aTexture, const glm::vec3& aNormal, const glm::vec3& aTangent, const glm::vec3& aBinormal, const glm::vec3& aColor)
  {
    mPosition = aPosition;
    mTexture = aTexture;
    mNormal = aNormal;
    mColor = aColor;
    mTangent = aTangent;
    mBinormal = aBinormal;
  }
};


#endif