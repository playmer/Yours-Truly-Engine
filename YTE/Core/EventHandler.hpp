#pragma once

#include <string>
#include <unordered_map>

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
      template <typename ObjectType, typename FunctionType, typename EventType>
      static void Caller(void *aObject, void *aFunction, Event *aEvent)
      {
        union FunctionMagic
        {
          FunctionType MemberFunction;
          void *VoidMemberFunction;
        };

        FunctionMagic magic;
        magic.VoidMemberFunction = aFunction;

        static_assert(std::is_base_of<Event, EventType>::value, "EventType Must be derived from YTE::Event");
        (static_cast<ObjectType*>(aObject)->*magic.MemberFunction)(static_cast<EventType*>(aEvent));
      }

      template <typename ObjectType, typename EventType>
      EventDelegate(ObjectType *aObject, void(ObjectType::*aFunction)(EventType*))
        : mObject(static_cast<void*>(aObject))
      {
        using FunctionType = void (ObjectType::*)(EventType*);

        static_assert(sizeof(void*) <= sizeof(FunctionType), "Please don't use multiple inheritance.");

        union FunctionMagic
        {
          FunctionType MemberFunction;
          void *VoidMemberFunction;
        };

        FunctionMagic magic;
        magic.MemberFunction = aFunction;

        mFunction = magic.VoidMemberFunction;

        mCallerFunction = Caller<ObjectType, void (ObjectType::*)(EventType*), EventType>;
      }

      void Invoke(Event *aEvent)
      {
        mCallerFunction(mObject, mFunction, aEvent);
      }

      using Invoker = void (*)(void*, void*, Event*);

      IntrusiveList<EventDelegate>::Hook mHook;
      void *mObject;
      void *mFunction;
      Invoker mCallerFunction;
    };

    template <typename ObjectType, typename EventType>
    void RegisterEvent(const std::string &aName, ObjectType *aObject, void (ObjectType::*aFunction)(EventType*))
    {
      auto delegate = aObject->MakeEventDelegate(aObject, aFunction);

      mEventLists[aName].InsertFront(delegate->mHook);
    }

    void DeregisterEvent(const std::string &aName)
    {

    }

    template <typename ObjectType, typename EventType>
    EventDelegate* MakeEventDelegate(ObjectType *aObject, void (ObjectType::*aFunction)(EventType*))
    {
      mHooks.emplace_back(aObject, aFunction);
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