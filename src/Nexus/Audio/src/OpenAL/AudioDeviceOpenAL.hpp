#pragma once
#include "Audio/AudioDevice.hpp"

#include <memory>

#include "OpenAL.hpp"

namespace Nexus::Audio
{
    struct ALCdeviceDeleter
    {
        void operator()(ALCdevice *device) const
        {
            if (device != nullptr)
            {
                alcCloseDevice(device);
                device = nullptr;
            }
        }
    };

    struct ALCcontextDeleter
    {
        void operator()(ALCcontext *context) const
        {
            if (context != nullptr)
            {
                alcDestroyContext(context);
                context = nullptr;
            }
        }
    };

    class AudioDeviceOpenAL final : public AudioDevice
    {
      public:
        AudioDeviceOpenAL();
        ~AudioDeviceOpenAL() final = default;
        AudioBufferHandle CreateAudioBuffer() final;
        AudioSourceHandle CreateAudioSource() final;
        void Play(AudioSourceHandle source) final;
        void Pause(AudioSourceHandle source) final;
        void Stop(AudioSourceHandle source) final;
        void Rewind(AudioSourceHandle source) final;

      private:
        std::unique_ptr<ALCdevice, ALCdeviceDeleter> m_Device = nullptr;
        std::unique_ptr<ALCcontext, ALCcontextDeleter> m_Context = nullptr;

        AudioResourceManager m_Resources = {};
    };
} // namespace Nexus::Audio