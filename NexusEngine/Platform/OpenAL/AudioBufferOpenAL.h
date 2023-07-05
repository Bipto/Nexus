#pragma once

#include "Core/Audio/AudioBuffer.h"

#include "OpenAL.h"

namespace Nexus::Audio
{
    class AudioBufferOpenAL : public AudioBuffer
    {
    public:
        AudioBufferOpenAL(ALsizei size, ALsizei frequency, ALenum format, ALvoid *data, ALboolean loop = AL_FALSE);
        virtual void *GetHandle() override;

    private:
        ALuint m_Buffer = 0;
    };
}