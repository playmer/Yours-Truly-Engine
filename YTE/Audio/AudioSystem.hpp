#pragma once

#include <filesystem>
#include <memory>
#include <unordered_map>

//#include "ga.h"
//#include "gau.h"

#include "YTE/Core/EventHandler.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Types.hpp"

struct Sound;
struct SoundHandle;

namespace YTE
{
  class AudioSystem : public EventHandler
  {
    struct SoundHolder
    {
      void operator()(Sound *aSound)
      {
        //Sound_release(aSound);
      }
    };

  public:

    struct SoundHandle
    {
      static void DeleteSound(SoundHandle* in_finishedHandle, void* in_context);

      SoundHandle();

      void Play();
    
      void Stop();
    
      // Between 0.0f and 1.0f
      void SetVolume(float aVolume);

      // Between 0.0f and 16.0f
      void SetPitch(float aPitch);

      // [-1.0 -> 0.0 -> 1.0]
      void SetPan(float aPan);

      operator bool()
      {
        return IsValid();
      }

      bool IsValid()
      {
        return nullptr != mHandle ? true : false;
      }
    
      SoundHandle *mHandle;
      //gau_SampleSourceLoop* mLoopSource;
      const char *mName;
    };

    AudioSystem(Engine *aEngine);

    ~AudioSystem();

    void Update(LogicUpdate *aUpdate)
    {
      //gau_manager_update(mManager);
    }

    std::unique_ptr<SoundHandle> PlayLoop(const std::string &aSoundName, float aVolume = 1.0f);

    std::unique_ptr<SoundHandle> PlayOnceManaged(const std::string &aSoundName, float aVolume = 1.0f);

    void PlayOnce(const std::string &aSoundName, float aVolume = 1.0f);

  private:
    std::unordered_map<std::string, std::unique_ptr<Sound, SoundHolder>> mSounds;
    //gau_Manager *mManager;
    //ga_Mixer *mMixer;
    //ga_StreamManager *mStreamManager;
    Engine *mEngine;
  };
}