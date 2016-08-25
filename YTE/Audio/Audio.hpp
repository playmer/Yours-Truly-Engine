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
      static void DeleteSound(ga_Handle* in_finishedHandle, void* in_context)
      {
        SoundHandle *handle = static_cast<SoundHandle*>(in_context);
        //ga_handle_release(handle->mHandle);
        ga_handle_destroy(handle->mHandle);
        handle->mHandle = nullptr;
      }

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
          printf("This handle, with the sound name of %s is null.", mName);
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
          printf("This handle, with the sound name of %s is null.", mName);
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
          printf("This handle, with the sound name of %s is null.", mName);
        }
      }

      // Between 0.0f and 16.0f
      void SetPitch(float aPitch)
      {
        if (nullptr != mHandle)
        {
          ga_handle_setParamf(mHandle, GA_HANDLE_PARAM_PITCH, aPitch);
        }
        else
        {
          printf("This handle, with the sound name of %s is null.", mName);
        }
      }

      // [-1.0 -> 0.0 -> 1.0]
      void SetPan(float aPan)
      {
        if (nullptr != mHandle)
        {
          ga_handle_setParamf(mHandle, GA_HANDLE_PARAM_PAN, aPan);
        }
        else
        {
          printf("This handle, with the sound name of %s is null.", mName);
        }
      }

      operator bool()
      {
        return IsValid();
      }

      bool IsValid()
      {
        return nullptr != mHandle ? true : false;
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

        mSounds.emplace(soundFilename, std::unique_ptr<ga_Sound, SoundHolder>(gau_load_sound_file(soundPath.c_str(), "wav"), SoundHolder()));
      }


      mMixer = gau_manager_mixer(mManager);
      mStreamManager = gau_manager_streamManager(mManager);
    }

    ~AudioManager()
    {
      gau_manager_destroy(mManager);
      gc_shutdown();
    }

    void Update(float aDt)
    {
      gau_manager_update(mManager);
    }

    std::unique_ptr<SoundHandle> PlayLoop(const std::string &aSoundName, float aVolume = 1.0f)
    {
      auto handle = std::make_unique<SoundHandle>();

      auto it = mSounds.find(aSoundName);

      if (it != mSounds.end())
      {
        auto sound = it->second.get();

        handle->mHandle = gau_create_handle_sound(mMixer, sound, &SoundHandle::DeleteSound, handle.get(), &handle->mLoopSource);

        gau_sample_source_loop_set(handle->mLoopSource, -1, 0);

        handle->mName = it->first.c_str();
        handle->SetVolume(aVolume);
        handle->Play();

      }
      else
      {
        printf("Couldn't find sound named %s", aSoundName.c_str());
      }

      return handle;
    }

    std::unique_ptr<SoundHandle> PlayOnceManaged(const std::string &aSoundName, float aVolume = 1.0f)
    {
      auto handle = std::make_unique<SoundHandle>();

      auto it = mSounds.find(aSoundName);

      if (it != mSounds.end())
      {
        auto sound = it->second.get();

        handle->mHandle = gau_create_handle_sound(mMixer, sound, &SoundHandle::DeleteSound, handle.get(), nullptr);

        handle->mName = it->first.c_str();
        handle->SetVolume(aVolume);
        handle->Play();
      }
      else
      {
        printf("Couldn't find sound named %s", aSoundName.c_str());
      }

      return handle;
    }

    void PlayOnce(const std::string &aSoundName, float aVolume = 1.0f)
    {
      auto it = mSounds.find(aSoundName);

      if (it != mSounds.end())
      {
        auto sound = it->second.get();

        SoundHandle handle;

        handle.mHandle = gau_create_handle_sound(mMixer, sound, gau_on_finish_destroy, nullptr, nullptr);

        handle.mName = it->first.c_str();
        handle.SetVolume(aVolume);
        handle.Play();
      }
      else
      {
        printf("Couldn't find sound named %s", aSoundName.c_str());
      }
    }

  private:
    std::unordered_map<std::string, std::unique_ptr<ga_Sound, SoundHolder>> mSounds;
    //std::vector<std::unique_ptr<SoundHandle>> mHandles;
    gau_Manager *mManager;
    ga_Mixer *mMixer;
    ga_StreamManager *mStreamManager;
  };
}