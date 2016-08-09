#pragma once

#include <filesystem>
#include <memory>
#include <unordered_map>

#include "ga.h"
#include "gau.h"

#include "YTE/Core/Types.hpp"

namespace YTE
{
  class AudioManager
  {
    struct SoundHolder
    {
      void operator()(ga_Sound *aSound)
      {
        ga_sound_release(aSound);
      }
    };

  public:
    struct SoundHandle
    {
      SoundHandle()
      {
        mHandle = nullptr;
        mName = nullptr;
      }

      void Play()
      {
        if (nullptr != mHandle)
        {
          ga_handle_play(mHandle);
        }
        else
        {
          printf("This handle is null.");
        }
      }
    
      void Stop()
      {
        if (nullptr != mHandle)
        {
          ga_handle_stop(mHandle);
        }
        else
        {
          printf("This handle is null.");
        }
      }
    
      // Between 0.0f and 1.0f
      void SetVolume(float aVolume)
      {
        if (nullptr != mHandle)
        {
          ga_handle_setParamf(mHandle, GA_HANDLE_PARAM_GAIN, aVolume);
        }
        else
        {
          printf("This handle is null.");
        }
      }
    
      ga_Handle *mHandle;
      gau_SampleSourceLoop* mLoopSource;
      const char *mName;
    };

    AudioManager()
    {
      gc_initialize(0);
      mManager = gau_manager_create();


      namespace fs = std::experimental::filesystem;

      for (auto& file : fs::directory_iterator(L"./Sounds"))
      {
        std::string soundPath = file.path().string();
        std::string soundFilename = file.path().stem().string();

        mSounds.emplace(soundFilename, gau_load_sound_file(soundPath.c_str(), "wav"));
      }


      mMixer = gau_manager_mixer(mManager);
      mStreamManager = gau_manager_streamManager(mManager);
    }

    ~AudioManager()
    {
      gau_manager_destroy(mManager);
      gc_shutdown();
    }

    //static void DeleteSound(ga_Handle* in_finishedHandle, void* in_context)
    //{
    //  SoundHandle *handle = static_cast<SoundHandle*>(in_context);
    //  handle->mManager;
    //}

    void Update(float aDt)
    {
      gau_manager_update(mManager);
    }

    // Only returns a valid SoundHandle when looping sound. Make sure to keep it alive if you're looping.
    std::unique_ptr<SoundHandle> Play(const std::string &aSoundName, float aVolume = 1.0f, bool aLooping = false)
    {
      auto it = mSounds.find(aSoundName);

      if (it != mSounds.end())
      {
        if (false == aLooping)
        {
          SoundHandle handle;
          handle.mHandle = gau_create_handle_sound(mMixer, it->second.get(), &gau_on_finish_destroy, 0, 0);
          
          handle.SetVolume(aVolume);
          handle.Play();

          return nullptr;
        }
        else
        {
          auto handle = std::make_unique<SoundHandle>();
          handle->mHandle = gau_create_handle_sound(mMixer, it->second.get(), 0, 0, &handle->mLoopSource);

          // Currently just looping the whole sound.
          gau_sample_source_loop_set(handle->mLoopSource, -1, 0);

          handle->SetVolume(aVolume);
          handle->Play();
        }
      }
      else
      {
        printf("Couldn't find sound named %s", aSoundName.c_str());
      }

      return nullptr;
    }

  private:
    std::unordered_map<std::string, std::unique_ptr<ga_Sound, SoundHolder>> mSounds;
    //std::vector<std::unique_ptr<SoundHandle>> mHandles;
    gau_Manager *mManager;
    ga_Mixer *mMixer;
    ga_StreamManager *mStreamManager;
  };
}