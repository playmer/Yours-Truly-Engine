#pragma once

#include "YTE/Core/ForwardDeclarations.hpp"

namespace YTE
{
  class Component
  {
    public:
    Component(Composition *aOwner);

    inline Composition* GetOwner() { return mOwner; }

    private:
    Composition *mOwner;
  };
}