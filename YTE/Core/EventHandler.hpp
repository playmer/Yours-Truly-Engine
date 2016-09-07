#pragma once

#include <string>
#include <unordered_map>

#include "YTE/Core/Types.hpp"

#include "YTE/DataStructures/IntrusiveList.hpp"

namespace YTE
{
  struct Event
  {

  };

  class EventHandler
  {
  public:
    class EventDelegate
    {
    public:
      using Invoker = void(*)(void*, Event*);

      template <typename ObjectType, typename FunctionType, FunctionType aFunction, typename EventType>
      static void Caller(void *aObject, Event *aEvent)
      {
        static_assert(std::is_base_of<Event, EventType>::value, "EventType Must be derived from YTE::Event");
        (static_cast<ObjectType*>(aObject)->*aFunction)(static_cast<EventType*>(aEvent));
      }

      template <typename ObjectType = EventHandler>
      EventDelegate(ObjectType *aObject, Invoker aInvoker)
        : mObject(static_cast<void*>(aObject))
      {
        mCallerFunction = aInvoker;
      }

      void Invoke(Event *aEvent)
      {
        mCallerFunction(mObject, aEvent);
      }

      IntrusiveList<EventDelegate>::Hook mHook;
      void *mObject;
      Invoker mCallerFunction;
    };

    template <typename FunctionType, FunctionType aFunction, typename EventType = Event, typename ObjectType = EventHandler>
    void RegisterEvent(const std::string &aName, ObjectType *aObject)
    {
      auto delegate = aObject->MakeEventDelegate<FunctionType, aFunction, ObjectType, EventType>(aObject);

      mEventLists[aName].InsertFront(delegate->mHook);
    }

    void DeregisterEvent(const std::string &aName)
    {
      runtime_assert(false, "This isn't implemented...");
    }

    template <typename FunctionType, FunctionType aFunction, typename ObjectType = EventHandler, typename EventType = Event>
    EventDelegate* MakeEventDelegate(ObjectType *aObject)
    {
      EventDelegate::Invoker callerFunction = EventDelegate::Caller<ObjectType, FunctionType, aFunction, EventType>;
      mHooks.emplace_back(aObject, callerFunction);
      mHooks.back().mObject = this;
      mHooks.back().mHook.mOwner = &mHooks.back();
      return &mHooks.back();
    }

    void SendEvent(const std::string &aName, Event *aEvent)
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

  private:
    std::unordered_map<std::string, IntrusiveList<EventDelegate>> mEventLists;
    std::vector<EventDelegate> mHooks;
  };


  struct LogicUpdate : public Event
  {
    float mDt;
  };
}