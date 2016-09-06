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
      Hook() : mHead(this), mPrevious(this), mNext(this), mOwner(nullptr)
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
      aHook.InsertAfter(mHead);
    }

    Hook mHead;
  };
}