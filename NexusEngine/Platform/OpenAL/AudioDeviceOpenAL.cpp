#include "AudioDeviceOpenAL.hpp"

#include "AudioBufferOpenAL.hpp"
#include "AudioSourceOpenAL.hpp"

#include "OpenAL.hpp"

#if !defined(EMSCRIPTEN)
#include "libnyquist/Common.h"
#include "libnyquist/Decoders.h"
#endif

namespace Nexus::Audio
{
    AudioDeviceOpenAL::AudioDeviceOpenAL()
    {
        // ALenum error = alGetError();
        ALCdevice *device = alcOpenDevice(nullptr);
        if (!device)
        {
            throw std::runtime_error("Failed to create audio device");
        }

        ALCcontext *context = alcCreateContext(device, nullptr);
        alcMakeContextCurrent(context);
    }

    AudioDeviceOpenAL::~AudioDeviceOpenAL()
    {
    }

#if !defined(EMSCRIPTEN)
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
#endif

    Ref<AudioBuffer> AudioDeviceOpenAL::CreateAudioBufferFromWavFile(const std::string &filepath)
    {
#if !defined(EMSCRIPTEN)
        nqr::WavDecoder decoder;

        nqr::AudioData data;
        decoder.LoadFromPath(&data, filepath);

        auto bitsPerSample = nqr::GetFormatBitsPerSample(data.sourceFormat);
        auto fileSize = data.samples.size() * sizeof(float);
        auto sampleRate = data.sampleRate;
        auto format = GetOpenALAudioFormat(data.sourceFormat, data.channelCount);
        auto dataPtr = (ALvoid *)data.samples.data();

        return CreateRef<AudioBufferOpenAL>(fileSize, sampleRate, format, dataPtr);
#else
        return nullptr;
#endif
    }

    Ref<AudioBuffer> AudioDeviceOpenAL::CreateAudioBufferFromMP3File(const std::string &filepath)
    {
#if !defined(EMSCRIPTEN)
        nqr::Mp3Decoder decoder;

        nqr::AudioData data;
        decoder.LoadFromPath(&data, filepath);

        auto bitsPerSample = nqr::GetFormatBitsPerSample(data.sourceFormat);
        auto fileSize = data.samples.size() * sizeof(float);
        auto sampleRate = data.sampleRate;
        auto format = GetOpenALAudioFormat(data.sourceFormat, data.channelCount);
        auto dataPtr = (ALvoid *)data.samples.data();

        return CreateRef<AudioBufferOpenAL>(fileSize, sampleRate, format, dataPtr);
#else
        return nullptr;
#endif
    }

    Ref<Audio::AudioSource> AudioDeviceOpenAL::CreateAudioSource(Ref<Audio::AudioBuffer> buffer)
    {
        return CreateRef<AudioSourceOpenAL>(buffer);
    }

    void AudioDeviceOpenAL::PlaySource(Ref<Audio::AudioSource> source)
    {
        Ref<AudioSourceOpenAL> s = std::dynamic_pointer_cast<Audio::AudioSourceOpenAL>(source);
        alSourcePlay(s->GetSource());
    }
}