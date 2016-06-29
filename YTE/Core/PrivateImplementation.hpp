//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef PrivateImplementation_hpp
#define PrivateImplementation_hpp

namespace YTE
{
  // Helper to capture the destructor of a type.
  template <typename T>
  void GenericDestruct(void *aMemory)
  {
    (reinterpret_cast<T*>(aMemory))->~T();
  }

  // Helper to call the constructor of a type.
  template <typename T>
  void GenericDefaultConstruct(void *aMemory)
  {
    new (aMemory) T();
  }

  template <int SizeInBytes>
  class PrivateImplementation
  {
  public:
    using Destructor = void(*)(void*);

    PrivateImplementation() : mDestructor(nullptr) {}

    ~PrivateImplementation()
    {
      // Destruct our data if it's already been constructed.
      if (mDestructor != nullptr)
      {
        mDestructor(mMemory);
      }
    }

    template <typename T>
    T* ConstructAndGet()
    {
      static_assert(sizeof(T) < SizeInBytes, "Constructed Type must be smaller than our size.");

      // Destruct our data if it's already been constructed.
      if (mDestructor != nullptr)
      {
        mDestructor(mMemory);
      }

      // Capture the destructor of the new type.
      mDestructor = GenericDestruct<T>;

      GenericDefaultConstruct<T>(mMemory);

      return reinterpret_cast<T*>(mMemory);
    }

    template <typename T>
    T* Get()
    {
      return reinterpret_cast<T*>(mMemory);
    }

  private:

    char mMemory[SizeInBytes];
    Destructor mDestructor;
  };
}
#endif
