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

    constexpr const char* Data() { return mData; };
    constexpr const char* data() { return mData; };
    constexpr size_t Size() { return mSize; };
    constexpr size_t size() { return mSize; };

    private:
    const char *mData;
    size_t mSize;
  };
}