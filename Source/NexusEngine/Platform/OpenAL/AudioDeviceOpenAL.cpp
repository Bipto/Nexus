#include "AudioDeviceOpenAL.hpp"

#include "AudioBufferOpenAL.hpp"
#include "AudioSourceOpenAL.hpp"

#include "OpenAL.hpp"

#include "libnyquist/Common.h"
#include "libnyquist/Decoders.h"

namespace Nexus::Audio
{
    AudioDeviceOpenAL::AudioDeviceOpenAL()
    {
        m_Device = alcOpenDevice(nullptr);
        if (!m_Device)
        {
            throw std::runtime_error("Failed to create audio device");
        }

        m_Context = alcCreateContext(m_Device, nullptr);
        alcMakeContextCurrent(m_Context);
    }

    AudioDeviceOpenAL::~AudioDeviceOpenAL()
    {
        alcDestroyContext(m_Context);
        alcCloseDevice(m_Device);
    }

    ALenum GetOpenALAudioFormat(nqr::PCMFormat format, int channelCount)
    {
        bool stereo = channelCount > 1;
        switch (format)
        {
            if (stereo)
            {
            case nqr::PCMFormat::PCM_U8:
            case nqr::PCMFormat::PCM_S8:
            {
                if (stereo)
                {
                    return AL_FORMAT_STEREO8;
                }
                else
                {
                    return AL_FORMAT_MONO8;
                }
            }
            case nqr::PCMFormat::PCM_16:
            case nqr::PCMFormat::PCM_24:
            {
                if (stereo)
                {
                    return AL_FORMAT_STEREO16;
                }
                else
                {
                    return AL_FORMAT_MONO16;
                }
            }
            case nqr::PCMFormat::PCM_32:
            case nqr::PCMFormat::PCM_64:
            case nqr::PCMFormat::PCM_FLT:
            case nqr::PCMFormat::PCM_DBL:
            {
                if (stereo)
                {
                    return AL_FORMAT_STEREO_FLOAT32;
                }
                else
                {
                    return AL_FORMAT_MONO_FLOAT32;
                }
            }
            }
        }
    }

    AudioBuffer *AudioDeviceOpenAL::CreateAudioBufferFromWavFile(const std::string &filepath)
    {
        nqr::WavDecoder decoder;

        nqr::AudioData data;
        decoder.LoadFromPath(&data, filepath);

        auto bitsPerSample = nqr::GetFormatBitsPerSample(data.sourceFormat);
        auto fileSize = data.samples.size() * sizeof(float);
        auto sampleRate = data.sampleRate;
        auto format = GetOpenALAudioFormat(data.sourceFormat, data.channelCount);
        auto dataPtr = (ALvoid *)data.samples.data();

        return new AudioBufferOpenAL(fileSize, sampleRate, format, dataPtr);
    }

    AudioBuffer *AudioDeviceOpenAL::CreateAudioBufferFromMP3File(const std::string &filepath)
    {
        nqr::Mp3Decoder decoder;

        nqr::AudioData data;
        decoder.LoadFromPath(&data, filepath);

        auto bitsPerSample = nqr::GetFormatBitsPerSample(data.sourceFormat);
        auto fileSize = data.samples.size() * sizeof(float);
        auto sampleRate = data.sampleRate;
        auto format = GetOpenALAudioFormat(data.sourceFormat, data.channelCount);
        auto dataPtr = (ALvoid *)data.samples.data();

        return new AudioBufferOpenAL(fileSize, sampleRate, format, dataPtr);
    }

    Audio::AudioSource *AudioDeviceOpenAL::CreateAudioSource(Audio::AudioBuffer *buffer)
    {
        return new AudioSourceOpenAL(buffer);
    }

    void AudioDeviceOpenAL::PlaySource(Audio::AudioSource *source)
    {
        AudioSourceOpenAL *s = (Audio::AudioSourceOpenAL *)source;
        alSourcePlay(s->GetSource());
    }
}