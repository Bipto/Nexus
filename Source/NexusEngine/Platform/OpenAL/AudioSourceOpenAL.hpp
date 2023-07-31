#pragma once

#include "Core/Audio/AudioSource.hpp"
#include "Core/Audio/AudioBuffer.hpp"

#include "Core/Memory.hpp"

#include "OpenAL.hpp"

namespace Nexus::Audio
{
    class AudioSourceOpenAL : public AudioSource
    {
    public:
        AudioSourceOpenAL() = delete;
        AudioSourceOpenAL(Ref<AudioBuffer> buffer);
        virtual glm::vec3 &GetPosition() override;
        virtual void SetPosition(const glm::vec3 &position) override;
        ALuint GetSource() { return m_Source; }

    private:
        glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};
        Ref<AudioBuffer> m_Buffer = nullptr;
        ALuint m_Source = 0;
    };
}