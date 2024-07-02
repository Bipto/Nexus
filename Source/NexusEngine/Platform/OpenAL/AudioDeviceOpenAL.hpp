#pragma once

#if defined(NX_PLATFORM_OPENAL)

#include "Nexus/Audio/AudioDevice.hpp"

#include "OpenAL.hpp"

#include "Nexus/nxpch.hpp"

namespace Nexus::Audio
{
    class AudioDeviceOpenAL : public AudioDevice
    {
    public:
        AudioDeviceOpenAL();
        virtual ~AudioDeviceOpenAL();
        virtual AudioAPI GetAPI() override { return AudioAPI::OpenAL; }
        virtual Ref<AudioBuffer> CreateAudioBufferFromWavFile(const std::string &filepath) override;
        virtual Ref<AudioBuffer> CreateAudioBufferFromMP3File(const std::string &filepath) override;
        virtual Ref<AudioSource> CreateAudioSource(Ref<AudioBuffer> buffer) override;
        virtual void PlaySource(Ref<AudioSource> source) override;

    private:
        ALCdevice *m_Device = nullptr;
        ALCcontext *m_Context = nullptr;
    };
}

#endif