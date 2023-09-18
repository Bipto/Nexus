#pragma once

#include "Nexus/Audio/AudioDevice.hpp"

#include "OpenAL.hpp"

#include <vector>
#include <string>

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
    };
}