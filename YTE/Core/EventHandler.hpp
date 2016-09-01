#pragma once

#include <string>
#include <unordered_map>

namespace YTE
{
  template <typename TemplateType>
  class IntrusiveList
  {
  public:
    //template <unsigned int Offset>
    class Hook
    {
    public:
      Hook()
        : mHead(this), mPrevious(this), mNext(this), mOwner(nullptr)
      {

      }

      Hook(TemplateType &aOwner)
        : mHead(this), mPrevious(this), mNext(this), mOwner(&aOwner)
      {

      }

      ~Hook()
      {
        Remove();
      }

      void Remove()
      {
        mPrevious->mNext = mNext;
        mNext->mPrevious = mPrevious;

        mHead = this;
        mPrevious = this;
        mNext = this;
      }

      void InsertAfter(Hook &aHook)
      {
        Remove();

        mPrevious = &aHook;
        mNext = aHook.mNext;
        aHook.mNext = this;
        
        mNext->mPrevious = this;

        mHead = aHook.mHead;
      }

      Hook *mHead;
      Hook *mPrevious;
      Hook *mNext;
      TemplateType *mOwner;
    };

    class iterator : public std::iterator<std::bidirectional_iterator_tag, TemplateType>
    {
    public:
      friend class const_iterator;
      friend class IntrusiveList<TemplateType>;

      inline iterator(Hook *aHook = nullptr)
        : mCurrent(aHook)
      {

      }

      inline iterator(const iterator &aIterator)
        : mCurrent(aIterator.mCurrent)
      {

      }

      inline iterator& operator=(const iterator &aIterator)
      {
        aIterator.mCurrent = mCurrent;
        return *this;
      }

      inline iterator& operator++()
      {
        mCurrent = mCurrent->mNext;
        return *this;
      }

      inline iterator operator++(int)
      {
        iterator previousIter{ *this };
        mCurrent = mCurrent->mNext;
        return previousIter;
      }

      inline bool operator==(const iterator &aIterator)
      {
        return mCurrent == aIterator.mCurrent;
      }

      inline bool operator!=(const iterator &aIterator)
      {
        return mCurrent != aIterator.mCurrent;
      }

      inline reference operator*()
      {
        return *static_cast<TemplateType*>(mCurrent->mOwner);
      }

      inline pointer operator->()
      {
        return static_cast<TemplateType*>(mCurrent->mOwner);
      }

    private:
      Hook *mCurrent;
    };

    iterator begin()
    {
      return iterator(mHead.mNext);
    }

    iterator end()
    {
      return iterator(&mHead);
    }

    void InsertFront(Hook &aHook)
    {
      mHead.InsertAfter(aHook);
    }

  private:
    Hook mHead;
  };

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
      return &mHooks.back();
    }

    void SendEvent(const std::string &aName, Event *aEvent)
    {
      auto it = mEventLists.find(aName);

      if (it != mEventLists.end())
      {
        for (auto eventDelegate : it->second)
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