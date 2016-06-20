//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef PrivateImplementation_hpp
#define PrivateImplementation_hpp

namespace YTE
{
  template <int SizeInBytes>
  class PrivateImplementation
  {
  public:
    using Destructor = void(*)();

    PrivateImplementation() : mDestructor(nullptr) {}

    template <typename T>
    T* ConstructAndGet()
    {
      // Destruct our data if it's already been constructed.
      if (mDestructor != nullptr)
      {
        mDestructor();
      }

      // Capture the destructor of the new type.
      mDestructor = GenericDestruct<T>;

      GenericDefaultConstruct();

      return reinterpret_cast<T*>(mMemory);
    }

    template <typename T>
    T* Get()
    {
      return reinterpret_cast<T*>(mMemory);
    }

  private:
    // Helper to capture the destructor of a type.
    template <typename T>
    void GenericDestruct()
    {
      (reinterpret_cast<T*>(mMemory))->~T();
    }

    // Helper to call the constructor of a type.
    template <typename T>
    void GenericDefaultConstruct()
    {
      static_assert(sizeof(T) < SizeInBytes, "Constructed Type must be smaller than our size.");
      new (mMemory) T();
    }

    char mMemory[SizeInBytes];
    Destructor mDestructor;
  };
}
#endif
