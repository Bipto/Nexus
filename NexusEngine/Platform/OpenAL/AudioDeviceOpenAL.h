#pragma once

#include "Core/Audio/AudioDevice.h"

#include "OpenAL.h"

#include <vector>
#include <string>

namespace Nexus
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