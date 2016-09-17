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
      Hook() : mPrevious(this), mNext(this), mOwner(nullptr)
      {

      }

      Hook(TemplateType &aOwner)
        : mPrevious(this), mNext(this), mOwner(&aOwner)
      {

      }

      ~Hook()
      {
        Unlink();
      }

      void Unlink()
      {
        mPrevious->mNext = mNext;
        mNext->mPrevious = mPrevious;

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