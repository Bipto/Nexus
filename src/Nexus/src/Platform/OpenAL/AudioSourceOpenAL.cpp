#include "AudioSourceOpenAL.hpp"

#include "AudioBufferOpenAL.hpp"

#if defined(NX_PLATFORM_OPENAL)

namespace Nexus::Audio
{
	AudioSourceOpenAL::AudioSourceOpenAL()
	{
		alGenSources(1, &m_Source);
	}

	AudioSourceOpenAL::~AudioSourceOpenAL()
	{
		alDeleteSources(1, &m_Source);
	}

	void AudioSourceOpenAL::SetPitch(float pitch)
	{
		alSourcef(m_Source, AL_PITCH, pitch);
	}

	void AudioSourceOpenAL::SetGain(float gain)
	{
		alSourcef(m_Source, AL_GAIN, gain);
	}

	void AudioSourceOpenAL::SetMaxDistance(float maxDistance)
	{
		alSourcef(m_Source, AL_MAX_DISTANCE, maxDistance);
	}

	void AudioSourceOpenAL::SetRolloffFactor(float rolloff)
	{
		alSourcef(m_Source, AL_ROLLOFF_FACTOR, rolloff);
	}

	void AudioSourceOpenAL::SetReferenceDistance(float reference)
	{
		alSourcef(m_Source, AL_REFERENCE_DISTANCE, reference);
	}

	void AudioSourceOpenAL::SetMinGain(float minGain)
	{
		alSourcef(m_Source, AL_MIN_GAIN, minGain);
	}

	void AudioSourceOpenAL::SetMaxGain(float maxGain)
	{
		alSourcef(m_Source, AL_MAX_GAIN, maxGain);
	}

	void AudioSourceOpenAL::SetConeOuterGain(float outerGain)
	{
		alSourcef(m_Source, AL_CONE_OUTER_GAIN, outerGain);
	}

	void AudioSourceOpenAL::SetConeInnerAngle(float innerAngle)
	{
		alSourcef(m_Source, AL_CONE_INNER_ANGLE, innerAngle);
	}

	void AudioSourceOpenAL::SetConeOuterAngle(float outerAngle)
	{
		alSourcef(m_Source, AL_CONE_OUTER_ANGLE, outerAngle);
	}

	void AudioSourceOpenAL::SetPosition(const glm::vec3 &position)
	{
		alSource3f(m_Source, AL_POSITION, position.x, position.y, position.z);
	}

	void AudioSourceOpenAL::SetVelocity(const glm::vec3 &velocity)
	{
		alSource3f(m_Source, AL_POSITION, velocity.x, velocity.y, velocity.z);
	}

	void AudioSourceOpenAL::SetDirection(const glm::vec3 &direction)
	{
		alSource3f(m_Source, AL_POSITION, direction.x, direction.y, direction.z);
	}

	void AudioSourceOpenAL::SetIsRelative(bool isRelative)
	{
		if (isRelative)
		{
			alSourcei(m_Source, AL_SOURCE_RELATIVE, AL_TRUE);
		}
		else
		{
			alSourcei(m_Source, AL_SOURCE_RELATIVE, AL_FALSE);
		}
	}

	void AudioSourceOpenAL::SetIsLooping(bool isLooping)
	{
		if (isLooping)
		{
			alSourcei(m_Source, AL_LOOPING, AL_TRUE);
		}
		else
		{
			alSourcei(m_Source, AL_LOOPING, AL_FALSE);
		}
	}

	void AudioSourceOpenAL::SetPlaybackPositionInSeconds(float seconds)
	{
		alSourcef(m_Source, AL_SEC_OFFSET, seconds);
	}

	void AudioSourceOpenAL::SetPlaybackPositionInSamples(float samples)
	{
		alSourcef(m_Source, AL_SAMPLE_OFFSET, samples);
	}

	void AudioSourceOpenAL::SetPlaybackPositionInBytes(float bytes)
	{
		alSourcef(m_Source, AL_BYTE_OFFSET, bytes);
	}

	void AudioSourceOpenAL::SetStaticSourceBuffer(Ref<AudioBuffer> buffer)
	{
		Ref<AudioBufferOpenAL> alBuffer = std::dynamic_pointer_cast<AudioBufferOpenAL>(buffer);
		alSourcei(m_Source, AL_BUFFER, (ALint)alBuffer->GetHandle());
		m_StaticBuffer = buffer;
	}

	void AudioSourceOpenAL::QueueBuffer(Ref<AudioBuffer> buffer)
	{
		Ref<AudioBufferOpenAL> alBuffer = std::dynamic_pointer_cast<AudioBufferOpenAL>(buffer);
		ALuint				   handle	= alBuffer->GetHandle();
		alSourceQueueBuffers(m_Source, 1, &handle);
	}

	void AudioSourceOpenAL::UnqueueBuffer(Ref<AudioBuffer> buffer)
	{
		Ref<AudioBufferOpenAL> alBuffer = std::dynamic_pointer_cast<AudioBufferOpenAL>(buffer);
		ALuint				   handle	= alBuffer->GetHandle();
		alSourceUnqueueBuffers(m_Source, 1, &handle);
	}

	void AudioSourceOpenAL::ClearAllBuffers()
	{
		alSourcei(m_Source, AL_BUFFER, AL_NONE);
		m_StaticBuffer = nullptr;
	}

	float AudioSourceOpenAL::GetPitch() const
	{
		float pitch;
		alGetSourcef(m_Source, AL_PITCH, &pitch);
		return pitch;
	}

	float AudioSourceOpenAL::GetGain() const
	{
		float gain;
		alGetSourcef(m_Source, AL_GAIN, &gain);
		return gain;
	}

	float AudioSourceOpenAL::GetMaxDistance() const
	{
		float maxDistance;
		alGetSourcef(m_Source, AL_MAX_DISTANCE, &maxDistance);
		return maxDistance;
	}

	float AudioSourceOpenAL::GetRolloffFactor() const
	{
		float rolloffFactor;
		alGetSourcef(m_Source, AL_ROLLOFF_FACTOR, &rolloffFactor);
		return rolloffFactor;
	}

	float AudioSourceOpenAL::GetReferenceDistance() const
	{
		float referenceDistance;
		alGetSourcef(m_Source, AL_REFERENCE_DISTANCE, &referenceDistance);
		return referenceDistance;
	}

	float AudioSourceOpenAL::GetMinGain() const
	{
		float minGain;
		alGetSourcef(m_Source, AL_MIN_GAIN, &minGain);
		return minGain;
	}

	float AudioSourceOpenAL::GetMaxGain() const
	{
		float maxGain;
		alGetSourcef(m_Source, AL_MAX_GAIN, &maxGain);
		return maxGain;
	}

	float AudioSourceOpenAL::GetConeOuterGain() const
	{
		float outerGain;
		alGetSourcef(m_Source, AL_CONE_OUTER_GAIN, &outerGain);
		return outerGain;
	}

	float AudioSourceOpenAL::GetConeInnerAngle() const
	{
		float innerAngle;
		alGetSourcef(m_Source, AL_CONE_INNER_ANGLE, &innerAngle);
		return innerAngle;
	}

	float AudioSourceOpenAL::GetConeOuterAngle() const
	{
		float outerAngle;
		alGetSourcef(m_Source, AL_CONE_OUTER_ANGLE, &outerAngle);
		return outerAngle;
	}

	glm::vec3 AudioSourceOpenAL::GetPosition() const
	{
		ALfloat position[3];
		alGetSourcefv(m_Source, AL_POSITION, position);
		return {position[0], position[1], position[2]};
	}

	glm::vec3 AudioSourceOpenAL::GetVelocity() const
	{
		ALfloat velocity[3];
		alGetSourcefv(m_Source, AL_VELOCITY, velocity);
		return {velocity[0], velocity[1], velocity[2]};
	}

	glm::vec3 AudioSourceOpenAL::GetDirection() const
	{
		ALfloat direction[3];
		alGetSourcefv(m_Source, AL_DIRECTION, direction);
		return {direction[0], direction[1], direction[2]};
	}

	bool AudioSourceOpenAL::GetIsRelative() const
	{
		ALint isRelative;
		alGetSourcei(m_Source, AL_SOURCE_RELATIVE, &isRelative);

		if (isRelative == AL_TRUE)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	SourceType AudioSourceOpenAL::GetSourceType() const
	{
		ALint sourceType;
		alGetSourcei(m_Source, AL_SOURCE_TYPE, &sourceType);
		return AL::GetSourceType(sourceType);
	}

	bool AudioSourceOpenAL::GetIsLooping() const
	{
		ALint isLooping;
		alGetSourcei(m_Source, AL_LOOPING, &isLooping);

		if (isLooping == AL_TRUE)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	float AudioSourceOpenAL::GetPlaybackPositionInSeconds() const
	{
		float position;
		alGetSourcef(m_Source, AL_SEC_OFFSET, &position);
		return position;
	}

	float AudioSourceOpenAL::GetPlaybackPositionInSamples() const
	{
		float position;
		alGetSourcef(m_Source, AL_SAMPLE_OFFSET, &position);
		return position;
	}

	float AudioSourceOpenAL::GetPlaybackPositionInBytes() const
	{
		float position;
		alGetSourcef(m_Source, AL_BYTE_OFFSET, &position);
		return position;
	}

	Ref<AudioBuffer> AudioSourceOpenAL::GetStaticSourceBuffer() const
	{
		return m_StaticBuffer;
	}

	SourceState AudioSourceOpenAL::GetSourceState() const
	{
		ALint sourceState;
		alGetSourcei(m_Source, AL_SOURCE_STATE, &sourceState);
		return AL::GetSourceState(sourceState);
	}

	size_t AudioSourceOpenAL::GetNumQueuedBuffers() const
	{
		ALint numBuffers;
		alGetSourcei(m_Source, AL_BUFFERS_QUEUED, &numBuffers);
		return static_cast<size_t>(numBuffers);
	}

	size_t AudioSourceOpenAL::GetNumProcessedBuffers() const
	{
		ALint numBuffers;
		alGetSourcei(m_Source, AL_BUFFERS_PROCESSED, &numBuffers);
		return static_cast<size_t>(numBuffers);
	}

	ALuint AudioSourceOpenAL::GetSource() const
	{
		return m_Source;
	}

}	 // namespace Nexus::Audio

#endif