#include "AudioBufferOpenAL.hpp"

#if defined(NX_PLATFORM_OPENAL)

#include "Nexus/nxpch.hpp"

void CheckError()
{
    ALenum error = alGetError();
    if (error != AL_NO_ERROR)
    {
        auto message = alGetString(error);
        std::cout << message << std::endl;
    }
}

namespace Nexus::Audio
{
    AudioBufferOpenAL::AudioBufferOpenAL(ALsizei size, ALsizei frequency, ALenum format, ALvoid *data, ALboolean loop)
    {
        alGenBuffers(1, &m_Buffer);
        CheckError();

        alBufferData(m_Buffer, format, data, size, frequency);
        CheckError();
    }

    AudioBufferOpenAL::~AudioBufferOpenAL()
    {
        alDeleteBuffers(1, &m_Buffer);
    }

    const ALuint AudioBufferOpenAL::GetHandle() const
    {
        return m_Buffer;
    }
}

#endif