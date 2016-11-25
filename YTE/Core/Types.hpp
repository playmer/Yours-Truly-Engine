//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef Types_hpp
#define Types_hpp

#include <stdint.h>

#include <memory>
#include <unordered_map>

#include "YTE/Core/ForwardDeclarations.hpp"

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
}
#endif