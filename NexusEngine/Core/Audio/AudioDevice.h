#pragma once

#include "Core/Memory.h"
#include "AudioBuffer.h"
#include "AudioSource.h"

#include <string>

namespace Nexus
{
    enum AudioAPI
    {
        OpenAL
    };

    class AudioDevice
    {
    public:
        virtual ~AudioDevice() {}
        virtual AudioAPI GetAPI() = 0;
        virtual Ref<AudioBuffer> CreateAudioBufferFromWavFile(const std::string &filepath) = 0;
        virtual Ref<AudioBuffer> CreateAudioBufferFromMP3File(const std::string &filepath) = 0;
        virtual Ref<AudioSource> CreateAudioSource(Ref<AudioBuffer> buffer) = 0;
        virtual void PlaySource(Ref<AudioSource> source) = 0;
    };
}