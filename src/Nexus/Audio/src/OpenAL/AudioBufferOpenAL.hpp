#pragma once

#include <functional>

#include "Audio/AudioBuffer.hpp"

#include "Core/AutoRelease.hpp"

#include "OpenAL.hpp"

namespace Nexus::Audio
{
    class AudioDeviceOpenAL;

    class AudioBufferOpenAL final : public IAudioBuffer
    {
      public:
        AudioBufferOpenAL(AudioDeviceOpenAL *device);
        ~AudioBufferOpenAL() final = default;
        const ALuint GetHandle() const;
        virtual void SetData(
            const void *const data, size_t size, AudioFormat format, size_t frequency
        ) override;
        virtual size_t GetFrequency() const override;
        virtual size_t GetBits() const override;
        virtual size_t GetChannels() const override;
        virtual size_t GetSize() const override;

      private:
        AudioDeviceOpenAL *m_Device = nullptr;
        AutoRelease<ALuint, 0, std::function<void(ALuint)>> m_Buffer = {};
    };
} // namespace Nexus::Audio