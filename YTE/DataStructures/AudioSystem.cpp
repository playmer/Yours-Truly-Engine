
#include "YTE/Audio/AudioSystem.hpp"

#include "YTE/Core/Engine.hpp"

namespace YTE
{
  void AudioSystem::SoundHandle::DeleteSound(ga_Handle * in_finishedHandle, void * in_context)
  {
    SoundHandle *handle = static_cast<SoundHandle*>(in_context);
    
    ga_handle_destroy(handle->mHandle);
    handle->mHandle = nullptr;
  }

  AudioSystem::SoundHandle::SoundHandle()
  {
    mHandle = nullptr;
    mName = nullptr;
  }

  void AudioSystem::SoundHandle::Play()
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

  void AudioSystem::SoundHandle::Stop()
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

  void AudioSystem::SoundHandle::SetVolume(float aVolume)
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

  void AudioSystem::SoundHandle::SetPitch(float aPitch)
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

  void AudioSystem::SoundHandle::SetPan(float aPan)
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

  AudioSystem::AudioSystem(Engine *aEngine) : mEngine(aEngine)
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


    static const std::string update = "LogicUpdate";
    mEngine->RegisterEvent<decltype(&AudioSystem::Update), &AudioSystem::Update, LogicUpdate>(update, this);
  }

  AudioSystem::~AudioSystem()
  {
    gau_manager_destroy(mManager);
    gc_shutdown();
  }

  std::unique_ptr<AudioSystem::SoundHandle> AudioSystem::PlayLoop(const std::string & aSoundName, float aVolume)
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
  
  std::unique_ptr<AudioSystem::SoundHandle> AudioSystem::PlayOnceManaged(const std::string & aSoundName, float aVolume)
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
  
  void AudioSystem::PlayOnce(const std::string & aSoundName, float aVolume)
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
}