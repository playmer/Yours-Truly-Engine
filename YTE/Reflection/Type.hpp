#pragma once

#include <string>
#include <vector>

#include "constexpr/cx_murmur3.h"

#include "YTE/Core/Types.hpp"

namespace YTE
{
  class Type;

  struct Name
  {
    u64 mHash;
    const char *mName;

    constexpr Name(const char *aName)
      : mHash(cx::murmur3_32(mName, 0x9747b28c)),
        mName(aName)
    {

    }

    bool operator ==(const Name &aName)
    {
      bool toReturn = false;

      if (mHash == aName.mHash)
      {
        if (mName != aName.mName)
        {
          toReturn = (0 == strcmp(mName, aName.mName)) ? true : false;
        }
        else
        {
          toReturn = true;
        }
      }

      return toReturn;
    }
  };

  class Field
  {
    std::string mName;
    Type *mType;
  };

  class Type
  {
    Name mName;
    std::vector<Field> mFields;
  };
}