#pragma once

#if defined(NX_PLATFORM_OPENAL)

	#include "Nexus-Core/Audio/AudioBuffer.hpp"
	#include "Nexus-Core/Audio/AudioSource.hpp"
	#include "Nexus-Core/Types.hpp"
	#include "OpenAL.hpp"

namespace Nexus::Audio
{
	class AudioSourceOpenAL : public AudioSource
	{
	  public:
		AudioSourceOpenAL();
		virtual ~AudioSourceOpenAL();

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
		void SetPosition(const glm::vec3 &position) final;
		void SetVelocity(const glm::vec3 &velocity) final;
		void SetDirection(const glm::vec3 &direction) final;
		void SetIsRelative(bool isRelative) final;
		void SetIsLooping(bool isLooping) final;
		void SetPlaybackPositionInSeconds(float seconds) final;
		void SetPlaybackPositionInSamples(float samples) final;
		void SetPlaybackPositionInBytes(float bytes) final;
		void SetStaticSourceBuffer(Ref<AudioBuffer> buffer) final;
		void QueueBuffer(Ref<AudioBuffer> buffer) final;
		void UnqueueBuffer(Ref<AudioBuffer> buffer) final;
		void ClearAllBuffers() final;

		float			 GetPitch() const final;
		float			 GetGain() const final;
		float			 GetMaxDistance() const final;
		float			 GetRolloffFactor() const final;
		float			 GetReferenceDistance() const final;
		float			 GetMinGain() const final;
		float			 GetMaxGain() const final;
		float			 GetConeOuterGain() const final;
		float			 GetConeInnerAngle() const final;
		float			 GetConeOuterAngle() const final;
		glm::vec3		 GetPosition() const final;
		glm::vec3		 GetVelocity() const final;
		glm::vec3		 GetDirection() const final;
		bool			 GetIsRelative() const final;
		SourceType		 GetSourceType() const final;
		bool			 GetIsLooping() const final;
		float			 GetPlaybackPositionInSeconds() const final;
		float			 GetPlaybackPositionInSamples() const final;
		float			 GetPlaybackPositionInBytes() const final;
		Ref<AudioBuffer> GetStaticSourceBuffer() const final;
		SourceState		 GetSourceState() const final;
		size_t			 GetNumQueuedBuffers() const final;
		size_t			 GetNumProcessedBuffers() const final;

		ALuint GetSource() const;

	  private:
		glm::vec3		 m_Position		= {0.0f, 0.0f, 0.0f};
		Ref<AudioBuffer> m_StaticBuffer = nullptr;
		ALuint			 m_Source		= 0;
	};
}	 // namespace Nexus::Audio

#endif