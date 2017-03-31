#pragma once

#include <forward_list>
#include <string>
#include <unordered_map>

#include "YTE/Core/Delegate.hpp"
#include "YTE/Core/Types.hpp"

#include "YTE/DataStructures/IntrusiveList.hpp"

#include "YTE/StandardLibrary/BlockAllocator.hpp"
#include "YTE/StandardLibrary/CompileTimeString.hpp"

namespace YTE
{
  struct Event
  {

  };

  class EventHandler
  {
  public:
    template <typename Return, typename Arg = Return>
    struct Binding {};

    template <typename Return, typename Object, typename Event>
    struct Binding<Return(Object::*)(Event*)>
    {
      using ReturnType = Return;
      using ObjectType = Object;
      using EventType = Event;
    };

    using DelegateType = Delegate<void(*)(Event*)>;
    using Invoker = DelegateType::Invoker;

    class EventDelegate : public DelegateType
    {
    public:
      // None of this for you.
      //EventDelegate(const EventDelegate &aDelegate) = delete;

      template <typename ObjectType = EventHandler>
      EventDelegate(ObjectType *aObject, Invoker aInvoker)
        : Delegate(aObject, aInvoker),
          mHook(this)
      {
      }

      EventDelegate(EventDelegate &&aEventDelegate)
        : Delegate(std::move(*this)), mHook(std::move(aEventDelegate.mHook), this)
      {
      }

      IntrusiveList<EventDelegate>::Hook mHook;
    };

    using Deleter = typename BlockAllocator<EventDelegate>::Deleter;
    using UniqueEvent = std::unique_ptr<EventDelegate, Deleter>;

    template <typename FunctionType, FunctionType aFunction, typename StringType = CompileTimeString, typename EventType = Event, typename ObjectType = EventHandler>
    void RegisterEvent(const StringType &aName, ObjectType *aObject)
    {
      static_assert(std::is_base_of<Event, EventType>::value, "EventType Must be derived from YTE::Event");
      static_assert(std::is_base_of<EventHandler, ObjectType>::value, "ObjectType Must be derived from YTE::EventHandler");
      auto delegate = aObject->template MakeEventDelegate<FunctionType,
                                                          aFunction,
                                                          typename StringType,
                                                          typename Binding<FunctionType>::ObjectType,
                                                          typename Binding<FunctionType>::EventType>(aName, aObject);

      mEventLists[aName].InsertFront(delegate->mHook);
    }

    void DeregisterEvent(const CompileTimeString &aName)
    {
      runtime_assert(false, "This isn't implemented...");
    }

    template <typename ObjectType, typename FunctionType, FunctionType aFunction, typename EventType>
    static void Caller(void *aObject, Event *aEvent)
    {
      (static_cast<ObjectType*>(aObject)->*aFunction)(static_cast<EventType*>(aEvent));
    }

    template <typename FunctionType, FunctionType aFunction, typename StringType = CompileTimeString, typename ObjectType = EventHandler, typename EventType = Event>
    EventDelegate* MakeEventDelegate(const StringType &aName, ObjectType *aObject)
    {
      static_assert(std::is_base_of<Event, EventType>::value, "EventType Must be derived from YTE::Event");
      Invoker callerFunction = Caller<ObjectType, FunctionType, aFunction, EventType>;
      auto &allocator = mDelegateAllocators[aName];
      auto ptr = allocator.allocate();

      new(ptr) EventDelegate(aObject, callerFunction);
      mHooks.emplace_back(std::move(UniqueEvent(ptr, allocator.GetDeleter())));
      return mHooks.back().get();
    }

    template <typename StringType = CompileTimeString>
    void SendEvent(const StringType &aName, Event *aEvent)
    {
      auto it = mEventLists.find(aName);

      if (it != mEventLists.end())
      {
        for (auto &eventDelegate : it->second)
        {
          eventDelegate.Invoke(aEvent);
        }
      }
    }

  protected:

    std::unordered_map<std::string, IntrusiveList<EventDelegate>> mEventLists;
    std::vector<UniqueEvent> mHooks;
    static std::unordered_map<std::string, BlockAllocator<EventDelegate>> mDelegateAllocators;
  };


  struct LogicUpdate : public Event
  {
    float mDt;
  };
}