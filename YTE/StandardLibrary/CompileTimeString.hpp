#pragma once
#include <stddef.h>

#include "Dependencies/constexpr/cx_utils.h"


namespace YTE
{
  class CompileTimeString
  {
    public:
    constexpr CompileTimeString() 
      : mData(nullptr), mSize(0) 
    {

    };

    constexpr CompileTimeString(const char *aData) 
      : mData(aData), mSize(cx::strlen(aData))
    {

    };



    private:
    const char *mData;
    size_t mSize;
  };
}