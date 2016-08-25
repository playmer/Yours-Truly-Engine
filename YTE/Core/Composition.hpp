#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "YTE/Core/Component.hpp"

#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Types.hpp"

namespace YTE
{
  class Composition
  {
    public:
    Composition(Composition *aParent);

    Component* GetComponent(const std::string &aName)
    {

    }

    private:
    ComponentMap mComponents;
    Composition *mParent;
  };
}