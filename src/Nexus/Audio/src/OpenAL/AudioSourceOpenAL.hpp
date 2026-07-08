#pragma once

#include <functional>

#include "Audio/AudioBuffer.hpp"
#include "Audio/AudioSource.hpp"

#include "Core/AutoRelease.hpp"

#include "OpenAL.hpp"
#include <Audio/AudioTypes.hpp>

namespace Nexus::Audio
{
    class NX_AUDIO_API AudioSourceOpenAL final : public IAudioSource
    {
      public:
        AudioSourceOpenAL();
        ~AudioSourceOpenAL() final = default;

        void SetPitch(float pitch) final;
        void SetGain(float gain) final;
        void SetMaxDistance(float maxDistance) final;
        void SetRolloffFactor(float rolloff) final;
        void SetReferenceDistance(float reference) final;
        void SetMinGain(float minGain) final;
        void SetMaxGain(float maxGain) final;
        void SetConeOuterGain(float outerGain) final;
        void SetConeInnerAngle(float innerAngle) final;
        void SetConeOuterAngle(float outerAngle) final;
        void SetPosition(float x, float y, float z) final;
        void SetVelocity(float x, float y, float z) final;
        void SetDirection(float x, float y, float z) final;
        void SetIsRelative(bool isRelative) final;
        void SetIsLooping(bool isLooping) final;
        void SetPlaybackPositionInSeconds(float seconds) final;
        void SetPlaybackPositionInSamples(float samples) final;
        void SetPlaybackPositionInBytes(float bytes) final;
        void SetStaticSourceBuffer(AudioBufferHandle buffer) final;
        void QueueBuffer(AudioBufferHandle buffer) final;
        void UnqueueBuffer(AudioBufferHandle buffer) final;
        void ClearAllBuffers() final;

        float GetPitch() const final;
        float GetGain() const final;
        float GetMaxDistance() const final;
        float GetRolloffFactor() const final;
        float GetReferenceDistance() const final;
        float GetMinGain() const final;
        float GetMaxGain() const final;
        float GetConeOuterGain() const final;
        float GetConeInnerAngle() const final;
        float GetConeOuterAngle() const final;
        std::tuple<float, float, float> GetPosition() const final;
        std::tuple<float, float, float> GetVelocity() const final;
        std::tuple<float, float, float> GetDirection() const final;
        bool GetIsRelative() const final;
        SourceType GetSourceType() const final;
        bool GetIsLooping() const final;
        float GetPlaybackPositionInSeconds() const final;
        float GetPlaybackPositionInSamples() const final;
        float GetPlaybackPositionInBytes() const final;
        AudioBufferHandle GetStaticSourceBuffer() const final;
        SourceState GetSourceState() const final;
        size_t GetNumQueuedBuffers() const final;
        size_t GetNumProcessedBuffers() const final;

        ALuint GetSource() const;

      private:
        AudioBufferHandle m_StaticBuffer = {};
        AutoRelease<ALuint, 0, std::function<void(ALuint)>> m_Source = {};
    };
} // namespace Nexus::Audio