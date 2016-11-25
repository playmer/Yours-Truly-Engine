#pragma once

#include "YTE/Core/EventHandler.hpp"

namespace YTE
{
  std::unordered_map<std::string, BlockAllocator<EventHandler::EventDelegate>> EventHandler::mDelegateAllocators;
}