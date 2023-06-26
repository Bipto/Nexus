#include "AudioBufferOpenAL.h"

#include <iostream>

void CheckError()
{
    ALenum error = alGetError();
    if (error != AL_NO_ERROR)
    {
        auto message = alGetString(error);
        std::cout << message << std::endl;
    }
}

namespace Nexus
{
    AudioBufferOpenAL::AudioBufferOpenAL(ALsizei size, ALsizei frequency, ALenum format, ALvoid *data, ALboolean loop)
    {
        alGenBuffers(1, &m_Buffer);
        CheckError();

        alBufferData(m_Buffer, format, data, size, frequency);
        CheckError();
    }

    void *AudioBufferOpenAL::GetHandle()
    {
        return (void *)m_Buffer;
    }
}
