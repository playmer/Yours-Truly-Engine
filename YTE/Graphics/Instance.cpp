#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/Instance.hpp"

namespace YTE
{
  void Instance::SetChanged(bool aChanged)
  {
    if (true == aChanged && false == mChanged)
    {
      mGraphicsSystem->mChangedInstances.emplace_back(this - mGraphicsSystem->mInstances.data());
    }

    mChanged = aChanged;
  }
}
