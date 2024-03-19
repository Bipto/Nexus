#pragma once

#if defined(NX_PLATFORM_OPENAL)

#include "Nexus/Audio/AudioSource.hpp"
#include "Nexus/Audio/AudioBuffer.hpp"

#include "Nexus/Types.hpp"

#include "OpenAL.hpp"

namespace Nexus::Audio
{
    class AudioSourceOpenAL : public AudioSource
    {
    public:
        AudioSourceOpenAL(Ref<AudioBuffer> buffer);
        virtual ~AudioSourceOpenAL();
        virtual glm::vec3 &GetPosition() override;
        virtual void SetPosition(const glm::vec3 &position) override;
        ALuint GetSource() { return m_Source; }

    private:
        glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};
        Ref<AudioBuffer> m_Buffer = nullptr;
        ALuint m_Source = 0;
    };
}

#endif