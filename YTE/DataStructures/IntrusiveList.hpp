#pragma once

#include <iterator>

namespace YTE
{
  template <typename TemplateType>
  class IntrusiveList
  {
  public:

    class Hook
    {
    public:
      // None of that for you.
      //Hook(const Hook &aHook) = delete;
      Hook& operator=(Hook &aHook) = delete;

      Hook() : mPrevious(this), mNext(this), mOwner(nullptr)
      {

      }

      Hook(TemplateType *aOwner) : mPrevious(this), mNext(this), mOwner(aOwner)
      {

      }

      Hook(TemplateType &aOwner)
        : mPrevious(this), mNext(this), mOwner(&aOwner)
      {

      }

      Hook(Hook &&aHook, TemplateType *aOwner = nullptr)
        : mPrevious(aHook.mPrevious), mNext(aHook.mNext), mOwner(aOwner)
      {
        runtime_assert(aOwner != nullptr, "We're moving a Hook node but not providing a new owner ptr.");
        aHook.Unlink();

        mPrevious->mNext = this;
        mNext->mPrevious = this;
      }

      ~Hook()
      {
        RemoveFromList();
      }

      void RemoveFromList()
      {
        mNext->mPrevious = mPrevious;
        mPrevious->mNext = mNext;
      }

      void Unlink()
      {
        RemoveFromList();

        mPrevious = this;
        mNext = this;
      }

      void InsertAfter(Hook &aHook)
      {
        Unlink();

        mPrevious = &aHook;
        mNext = aHook.mNext;
        aHook.mNext = this;

        mNext->mPrevious = this;
      }

      Hook *mPrevious;
      Hook *mNext;
      TemplateType *mOwner;
    };

    class iterator : public std::iterator<std::bidirectional_iterator_tag, TemplateType>
    {
    public:
      friend class const_iterator;
      friend class IntrusiveList<TemplateType>;

      using pointer = TemplateType*;
      using reference = TemplateType&;

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
      aHook.InsertAfter(mHead);
    }

    void UnlinkAll()
    {
      for (;;)
      {
        Hook *hook = mHead.mPrevious;

        if (hook == &mHead)
        {
          break;
        }

        hook->Unlink();
      }
    }

    Hook mHead;
  };
}