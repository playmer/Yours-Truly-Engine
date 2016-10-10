#pragma once

#include <string>
#include <unordered_map>

#include "YTE/Core/Delegate.hpp"
#include "YTE/Core/Types.hpp"

#include "YTE/DataStructures/IntrusiveList.hpp"

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

    class EventDelegate
    {
    public:
      // None of this for you.
      //EventDelegate(const EventDelegate &aDelegate) = delete;

      template <typename ObjectType = EventHandler>
      EventDelegate(ObjectType *aObject, Invoker aInvoker)
        : mHook(this),
        mDelegate(aObject, aInvoker)
      {
      }

      EventDelegate(EventDelegate &&aEventDelegate)
        : mHook(std::move(aEventDelegate.mHook), this),
          mDelegate(std::move(aEventDelegate.mDelegate))
      {
      }

      inline void Invoke(Event *aEvent)
      {
        mDelegate.Invoke(aEvent);
      }

      IntrusiveList<EventDelegate>::Hook mHook;
      DelegateType mDelegate;
    };

    template <typename FunctionType, FunctionType aFunction, typename StringType = CompileTimeString, typename EventType = Event, typename ObjectType = EventHandler>
    void RegisterEvent(const StringType &aName, ObjectType *aObject)
    {
      auto delegate = aObject->template MakeEventDelegate<FunctionType,
        aFunction,
        typename Binding<FunctionType>::ObjectType,
        typename Binding<FunctionType>::EventType>(aObject);

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

    template <typename FunctionType, FunctionType aFunction, typename ObjectType = EventHandler, typename EventType = Event>
    EventDelegate* MakeEventDelegate(ObjectType *aObject)
    {
      static_assert(std::is_base_of<Event, EventType>::value, "EventType Must be derived from YTE::Event");
      Invoker callerFunction = Caller<ObjectType, FunctionType, aFunction, EventType>;
      mHooks.emplace_back(aObject, callerFunction);
      return &mHooks.back();
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
    std::vector<EventDelegate> mHooks;
  };


  struct LogicUpdate : public Event
  {
    float mDt;
  };
}