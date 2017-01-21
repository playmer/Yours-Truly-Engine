//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef Types_hpp
#define Types_hpp

#include <stdint.h>

#include <memory>
#include <string>
#include <unordered_map>

#include "YTE/Core/ForwardDeclarations.hpp"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

namespace YTE
{
  using byte = std::uint8_t;

  using i8 =  std::int8_t;
  using i16 = std::int16_t;
  using i32 = std::int32_t;
  using i64 = std::int64_t;

  using u8  = std::uint8_t;
  using u16 = std::uint16_t;
  using u32 = std::uint32_t;
  using u64 = std::uint64_t;

  // The fact that I actually have to do this makes me sick.
  using namespace std::string_literals;

  using ComponentMap = std::unordered_map<std::string, std::unique_ptr<Component>>;


  // Helper to capture the destructor of a type.
  template <typename T>
  void GenericDestruct(void *aMemory)
  {
    (reinterpret_cast<T*>(aMemory))->~T();
  }

  // Helper to call the constructor of a type.
  template <typename T>
  void GenericDefaultConstruct(void *aMemory)
  {
    new (aMemory) T();
  }


  inline void runtime_assert(bool aValue, const char *aMessage = "")
  {
    if (false == aValue)
    {
      printf("ASSERT: %s\n", aMessage);

      // Purposely ruining this programs life.
      int *base = nullptr;
      *base = 1;
    }
  }


  glm::mat4 Rotate(glm::mat4 &aMatrix, glm::vec3 aRotation)
  {
    float xCos = cos(aRotation.x);
    float xSin = sin(aRotation.x);
    float yCos = cos(aRotation.y);
    float ySin = sin(aRotation.y);
    float zCos = cos(aRotation.z);
    float zSin = sin(aRotation.z);

    float ySin_zCos = ySin * zCos;

    glm::mat4 rotation(glm::uninitialize);
    rotation[0][0] = yCos * zCos;
    rotation[0][1] = -yCos * zSin;
    rotation[0][2] = ySin;

    rotation[1][0] = xCos * zSin + xSin * ySin_zCos;
    rotation[1][1] = xCos * zCos - xSin * ySin * zSin;
    rotation[1][2] = -xSin * yCos;

    rotation[2][0] = xSin * zSin - xCos * ySin_zCos;
    rotation[2][1] = xCos * ySin * zSin + xSin * zCos;
    rotation[2][2] = xCos * yCos;


    glm::mat4 result(glm::uninitialize);
    result[0] = aMatrix[0] * rotation[0][0] + 
                aMatrix[1] * rotation[0][1] + 
                aMatrix[2] * rotation[0][2];

    result[1] = aMatrix[0] * rotation[1][0] + 
                aMatrix[1] * rotation[1][1] + 
                aMatrix[2] * rotation[1][2];
    result[2] = aMatrix[0] * rotation[2][0] + 
                aMatrix[1] * rotation[2][1] + 
                aMatrix[2] * rotation[2][2];
    result[3] = aMatrix[3];

    return result;
  }
}
#endif