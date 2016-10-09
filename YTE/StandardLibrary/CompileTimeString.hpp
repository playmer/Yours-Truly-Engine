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

    constexpr const char* Data() const { return mData; };
    constexpr const char* data() const { return mData; };
    constexpr size_t Size() const { return mSize; };
    constexpr size_t size() const { return mSize; };
    constexpr size_t Hash() const { return mHash; };
    constexpr size_t hash() const { return mHash; };

    constexpr bool operator==(const CompileTimeString &aString) const
    {
      if (aString.mSize == mSize)
      {
        auto val = cx::strcmp(mData, aString.mData);

        return val == 0 ? true : false;
      }
    }

    private:
    size_t mHash;
    size_t mSize;
    const char *mData;
  };

  struct StringComparison
  {
    // We can get some optimizations if we write a custom function for this case.
    inline bool operator()(const CompileTimeString &aLeft, const CompileTimeString &aRight) const
    {
      if (aLeft.Data() == aRight.Data())
      {
        return true;
      }

      if (aLeft.Hash() == aRight.Hash())
      {
        if (aLeft.Size() == aRight.Size())
        {
          auto val = std::strcmp(aLeft.Data(), aRight.Data());

          return val == 0 ? true : false;
        }
      }
    }

    inline bool operator()(const CompileTimeString &aLeft, const std::string &aRight) const
    {
      if (aLeft.Data() == aRight.data())
      {
        return true;
      }

      if (aLeft.Size() == aRight.size())
      {
        auto val = std::strcmp(aLeft.Data(), aRight.data());

        return val == 0 ? true : false;
      }
    }

    inline bool operator()(const std::string &aLeft, const CompileTimeString &aRight) const
    {
      return (*this)(aRight, aLeft);
    }

    bool operator()(const CompileTimeString &aLeft, const char *aRight) const
    {
      auto val = std::strcmp(aLeft.Data(), aRight);

      return val == 0 ? true : false;
    }
    
    bool operator()(const char *aLeft, const CompileTimeString &aRight) const
    {
      return (*this)(aRight, aLeft);
    }
  };
}

namespace std
{
  template <>
  struct hash<YTE::CompileTimeString> : public unary_function<YTE::CompileTimeString, size_t>
  {
    size_t operator()(const YTE::CompileTimeString& value) const
    {
      return value.Hash();
    }
  };

  //template <>
  //struct equal_to<YTE::CompileTimeString> : public unary_function<YTE::String, bool>
  //{
  //  bool operator()(const YTE::CompileTimeString& x, const YTE::CompileTimeString& y) const
  //  {
  //    return x == y;
  //  }
  //};

} // namespace std