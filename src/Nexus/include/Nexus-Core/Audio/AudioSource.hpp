#pragma once

#include "AudioBuffer.hpp"
#include "AudioTypes.hpp"

#include "glm/glm.hpp"

namespace Nexus::Audio
{

	/// @brief A pure virtual class representing a source that is able to play audio
	class NX_API AudioSource
	{
	  public:
		/// @brief A virtual destructor to clean up resources
		virtual ~AudioSource()
		{
		}

		virtual void SetPitch(float pitch)							= 0;
		virtual void SetGain(float gain)							= 0;
		virtual void SetMaxDistance(float maxDistance)				= 0;
		virtual void SetRolloffFactor(float rolloff)				= 0;
		virtual void SetReferenceDistance(float reference)			= 0;
		virtual void SetMinGain(float minGain)						= 0;
		virtual void SetMaxGain(float maxGain)						= 0;
		virtual void SetConeOuterGain(float outerGain)				= 0;
		virtual void SetConeInnerAngle(float innerAngle)			= 0;
		virtual void SetConeOuterAngle(float outerAngle)			= 0;
		virtual void SetPosition(const glm::vec3 &position)			= 0;
		virtual void SetVelocity(const glm::vec3 &velocity)			= 0;
		virtual void SetDirection(const glm::vec3 &direction)		= 0;
		virtual void SetIsRelative(bool isRelative)					= 0;
		virtual void SetIsLooping(bool isLooping)					= 0;
		virtual void SetPlaybackPositionInSeconds(float seconds)	= 0;
		virtual void SetPlaybackPositionInSamples(float samples)	= 0;
		virtual void SetPlaybackPositionInBytes(float bytes)		= 0;
		virtual void SetStaticSourceBuffer(Ref<AudioBuffer> buffer) = 0;
		virtual void QueueBuffer(Ref<AudioBuffer> buffer)			= 0;
		virtual void UnqueueBuffer(Ref<AudioBuffer> buffer)			= 0;
		virtual void ClearAllBuffers()								= 0;

		virtual float			 GetPitch() const					  = 0;
		virtual float			 GetGain() const					  = 0;
		virtual float			 GetMaxDistance() const				  = 0;
		virtual float			 GetRolloffFactor() const			  = 0;
		virtual float			 GetReferenceDistance() const		  = 0;
		virtual float			 GetMinGain() const					  = 0;
		virtual float			 GetMaxGain() const					  = 0;
		virtual float			 GetConeOuterGain() const			  = 0;
		virtual float			 GetConeInnerAngle() const			  = 0;
		virtual float			 GetConeOuterAngle() const			  = 0;
		virtual glm::vec3		 GetPosition() const				  = 0;
		virtual glm::vec3		 GetVelocity() const				  = 0;
		virtual glm::vec3		 GetDirection() const				  = 0;
		virtual bool			 GetIsRelative() const				  = 0;
		virtual SourceType		 GetSourceType() const				  = 0;
		virtual bool			 GetIsLooping() const				  = 0;
		virtual float			 GetPlaybackPositionInSeconds() const = 0;
		virtual float			 GetPlaybackPositionInSamples() const = 0;
		virtual float			 GetPlaybackPositionInBytes() const	  = 0;
		virtual Ref<AudioBuffer> GetStaticSourceBuffer() const		  = 0;
		virtual SourceState		 GetSourceState() const				  = 0;
		virtual size_t			 GetNumQueuedBuffers() const		  = 0;
		virtual size_t			 GetNumProcessedBuffers() const		  = 0;
	};
}	 // namespace Nexus::Audio