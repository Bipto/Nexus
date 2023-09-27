#pragma once

#include "Nexus/Audio/AudioBuffer.hpp"

#include "OpenAL.hpp"

namespace Nexus::Audio
{
    class AudioBufferOpenAL : public AudioBuffer
    {
    public:
        AudioBufferOpenAL(ALsizei size, ALsizei frequency, ALenum format, ALvoid *data, ALboolean loop = AL_FALSE);
        virtual ~AudioBufferOpenAL();
        virtual void *GetHandle() override;

    private:
        ALuint m_Buffer = 0;
    };
}