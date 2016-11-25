#pragma once
#include <stddef.h>

#include "Dependencies/constexpr/cx_murmur3.h"
#include "Dependencies/constexpr/cx_utils.h"


namespace YTE
{
  class CompileTimeString
  {
    public:
    constexpr CompileTimeString() 
      : mData(nullptr), mSize(0), mHash(0)
    {

    };

    constexpr CompileTimeString(const char *aData) 
      : mHash(cx::murmur3_32(aData, 67280421310721)),
        mSize(cx::strlen(aData)),
        mData(aData)
    {

    };

    constexpr const char* Data() { return mData; };
    constexpr const char* data() { return mData; };
    constexpr size_t Size() { return mSize; };
    constexpr size_t size() { return mSize; };
    constexpr size_t Hash() { return mHash; };
    constexpr size_t hash() { return mHash; };

    constexpr bool operator==(const CompileTimeString &aString) const
    {
      if (mHash == aString.mHash && aString.mSize == mSize)
      {
        auto left = mData;
        auto right = aString.mData;
        while (*left)
        {
          if (*left != *right)
          {
            return false;
          }

          ++left;
          ++right;
        }

        return true;
      }
    }

    private:
    size_t mHash;
    size_t mSize;
    const char *mData;
  };
}