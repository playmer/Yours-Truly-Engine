#pragma once

#include <allocators>
#include <array>
#include <list>

namespace YTE
{
  template <typename T, size_t S = 128>
  class BlockAllocator : std::allocator<T>
  {
  public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    //propagate_on_container_move_assignment(C++14) std::true_type
    //rebind(deprecated in C++17)	template< class U > struct rebind { typedef allocator<U> other; };
    //is_always_equal(C++17) std::true_type

    using storage_type = typename std::aligned_storage<sizeof(value_type), alignof(value_type)>::type;
    using array_type = std::array<storage_type, S>;

    class Deleter
    {
    public:
      Deleter(BlockAllocator<value_type> *aAllocator) :
        mAllocator(aAllocator)
      {};

      void operator()(value_type *aToDelete)
      {
        mAllocator->deallocate(aToDelete);
      }

    private:
      BlockAllocator<value_type> *mAllocator;
    };

    Deleter GetDeleter()
    {
      return Deleter(this);
    }

    pointer allocate()
    {
      AllocateIfNeeded();

      return mFreeList.pop_front();
    }

    void deallocate(pointer aPointer)
    {
      mFreeList.push_front(aPointer);
    }

  private:
    class FreeList
    {
      struct Node
      {
        Node *mNext;
      };

    public:

      void push_front(pointer aPointer)
      {
        static_assert(sizeof(value_type) >= sizeof(Node),
                      "Type is not large enough to be in a block.");

        auto front = reinterpret_cast<Node*>(aPointer);

        front->mNext = mHead;
        mHead = front;

        ++mSize;
      }

      pointer pop_front()
      {
        auto front = mHead;
        mHead = mHead->mNext;

        --mSize;
        return reinterpret_cast<pointer>(front);
      }

      bool IsEmpty()
      {
        return mHead == nullptr;
      }

      size_t size() { return mSize; }

    private:

      size_t mSize;
      Node *mHead;
    };

    void AllocateIfNeeded()
    {
      mData.emplace_front(array_type());
      
      for (auto &i : mData.front())
      {
        pointer j = reinterpret_cast<pointer>(&i);

        mFreeList.push_front(j);
      }

    }

    std::list<array_type> mData;
    FreeList mFreeList;
    size_t mSize;
  };
}