#include "AudioSourceOpenAL.h"

namespace Nexus::Audio
{
    AudioSourceOpenAL::AudioSourceOpenAL(Ref<AudioBuffer> buffer)
    {
        m_Buffer = buffer;

        alGenSources(1, &m_Source);
        alSourcef(m_Source, AL_PITCH, 1);
        alSourcef(m_Source, AL_GAIN, 1.0f);
        alSource3f(m_Source, AL_POSITION, 0, 0, 0);
        alSource3f(m_Source, AL_VELOCITY, 0, 0, 0);
        alSourcei(m_Source, AL_LOOPING, AL_FALSE);
        alSourcei(m_Source, AL_BUFFER, (ALint)m_Buffer->GetHandle());

        if (alGetError() != AL_NO_ERROR)
        {
            throw std::runtime_error("Failed to create buffer");
        }
    }

    glm::vec3 &AudioSourceOpenAL::GetPosition()
    {
        return m_Position;
    }
    void AudioSourceOpenAL::SetPosition(const glm::vec3 &position)
    {
        m_Position = position;
    }
}