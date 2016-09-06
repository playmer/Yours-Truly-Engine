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
  using byte = unsigned char;

  using i8 = int8_t;
  using i16 = int16_t;
  using i32 = int32_t;
  using i64 = int64_t;

  using u8  = uint8_t;
  using u16 = uint16_t;
  using u32 = uint32_t;
  using u64 = uint64_t;

  using ComponentMap = std::unordered_map<std::string, std::unique_ptr<Component>>;


  inline void runtime_assert(bool aValue, char *aMessage = "")
  {
    if (false == aValue)
    {
      printf("ASSERT: %s\n", aMessage);

      int *base = 0;
      *base = 1;
    }
  }
}
#endif