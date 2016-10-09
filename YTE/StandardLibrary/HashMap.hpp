#pragma once
#ifndef YTE_StandardLibrary_HashMap
#define YTE_StandardLibrary_HashMap

#include <functional>

namespace YTE
{
  namespace Detail
  {
    template <class Key, class T>
    struct HashMapNode
    {
      Key mKey;
      T mData;
      HashMapNode *mNext;
      HashMapNode(const Key& aKey, const T& aData) 
        : mKey(aKey), mData(aData), mNext(nullptr) {};
    };
  }

  template <class Key,
            class T,
            class Hash = std::hash<Key>,
            class KeyEqual = std::equal_to<Key>,
            class Allocator = std::allocator<Detail::HashMapNode<Key,T>>,
            size_t InitialTableSize = 2,
            float MaxLoadFactor = 3.0,
            float GrowthFactor = 2.0
           >
  class HashMap
  {
    public:
    using Node = Detail::HashMapNode;

    // Each list has a special head pointer
    struct HeadNode
    {
      Node *mNodes;
      HeadNode() : mNodes(nullptr){};
    };

    HashMap()
    {
      // Set up the array.
      mArray = new HeadNode[InitialTableSize];
    }

    ~HashMap()
    {
      clear();

      delete[] mArray;
    }

    // Inserts a key/data pair into the hashmap.
    template <typename KeyType = Key>
    void insert(const KeyType &Key, const T& Data)
    {
      ResizeTable();
      InsertNode(MakeNode(Key, Data));
    }

    //  Delete an item by key.
    //  Compacts the table by moving key / data pairs, if necessary.
    template <typename KeyType = Key>
    void remove(const KeyType &aKey)
    {
      unsigned index{ mConfig.HashFunc_(aKey, mTableSize) };

      auto head = mArray + index;
      auto list = head->Nodes;
      Node *prev = nullptr;

      while (list)
      {
        if (KeyEqual(list->mKey, aKey) == 0)
        {
          // We've found it.
          RemoveNode(head, prev, list);
          return;
        }

        prev = list;
        list = list->Next;
      }
    }


    // Find and return data by key.
    template <typename KeyType = Key>
    const T& find(const KeyType &Key) const
    {
      unsigned index{ mConfig.HashFunc_(Key, mTableSize) };

      auto head = mArray + index;
      auto list = head->Nodes;

      while (list)
      {
        if (strcmp(list->Key, Key) == 0)
        {
          // We've found it.
          return list->Data;
        }

        list = list->Next;
      }

      // We didn't find it.
      throw HashTableException(HashTableException::E_ITEM_NOT_FOUND,
                               "Item not found!");
    }

    // Removes all items from the table (Doesn't deallocate table)
    void clear()
    {
      for (size_t i{ 0 }; i < mTableSize; ++i)
      {
        auto head = mArray + i;
        auto list = head->Nodes;

        while (list)
        {
          auto next = list->Next;

          DeleteNode(list);

          list = next;
        }

        head->Nodes = nullptr;
      }

      mSize = 0;
    }

    private:
    // Make a node.
    template <typename KeyType = Key>
    typename Node *MakeNode(const KeyType &aKey, const T& aData)
    {
      Node *node = mAllocator.allocate(1);

      if (nullptr != node)
      {
        new(node) Node(aKey, aData);
      }

      return node;
    }

    // Place a node into the hashtable.
    void InsertNode(Node *node)
    {
      unsigned index{ mConfig.HashFunc_(node->Key, mTableSize) };

      ListPushFront(mArray + index, node);
    }

    // Remove a node from the HashMap.
    void RemoveNode(HeadNode *aHead, Node *aPrevious, Node *aNode)
    {
      // Prev was head.
      if (aPrevious == nullptr)
      {
        aHead->Nodes = aNode->Next;
      }
      // Prev was a node.
      else
      {
        aPrevious->Next = aNode->Next;
      }

      DeleteNode(aNode);

      --mSize;
    }

    // Delete a node from the hashtable.
    void DeleteNode(Node *aNode)
    {
      aNode->~Node();
      mAllocator->deallocate(aNode, 1);
    }

    // Checks to see if the table needs to be resized, and does so if needed.
    void ResizeTable()
    {
      auto potentialSize = mSize + 1;
      double potentialRatio = static_cast<double>(potentialSize) /
                              static_cast<double>(mTableSize);

      if (potentialRatio > mConfig.MaxLoadFactor_)
      {
        auto oldArray = mArray;
        auto oldArraySize = mTableSize;

        // Make our replacement array.
        mTableSize = GetClosestPrime(static_cast<unsigned>(oldArraySize * GrowthFactor) + 1);
        mArray = new HeadNode[mTableSize];

        for (size_t i{ 0 }; i < oldArraySize; ++i)
        {
          auto list = oldArray[i].Nodes;

          while (list)
          {
            auto next = list->Next;
            list->Next = nullptr;
            InsertNode(list);

            list = next;
          }
        }

        delete[] oldArray;
      }
    }


    // Place a node into the front of the list.
    void ListPushFront(HeadNode *head, Node *node)
    {
      // Check for duplicates.
      auto list = head->Nodes;
      while (list)
      {
        // Check for duplicates
        if (strcmp(node->Key, list->Key) == 0)
        {
          DeleteNode(node);
          throw HashTableException(HashTableException::E_DUPLICATE,
                                   "Duplicate key found!");
        }

        list = list->Next;
      }

      node->Next = head->Nodes;

      head->Nodes = node;
    }

    size_t mTotalNodes = 0;
    size_t mTableSize = 0;
    size_t mSize = 0;

    // Array of head pointers.
    HeadNode *mArray;
    Allocator mAllocator;
  };
}


#endif
