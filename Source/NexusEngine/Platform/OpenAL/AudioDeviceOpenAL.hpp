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
        virtual AudioBuffer *CreateAudioBufferFromWavFile(const std::string &filepath) override;
        virtual AudioBuffer *CreateAudioBufferFromMP3File(const std::string &filepath) override;
        virtual AudioSource *CreateAudioSource(AudioBuffer *buffer) override;
        virtual void PlaySource(AudioSource *source) override;

    private:
        ALCdevice *m_Device = nullptr;
        ALCcontext *m_Context = nullptr;
    };
}