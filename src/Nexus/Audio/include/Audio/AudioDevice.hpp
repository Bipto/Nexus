#pragma once

#include <memory>

#include "Audio-Core.hpp"
#include "AudioBuffer.hpp"
#include "AudioSource.hpp"

namespace Nexus::Audio
{
	struct AudioResourceManager
	{
		AudioBufferPool AudioBuffers = {};
		AudioSourcePool AudioSources = {};
	};

	/// @brief A class representing an abstraction over a audio API and context
	class NX_AUDIO_API AudioDevice
	{
	  public:
		/// @brief Virtual destructor used to destroy any necessary data
		virtual ~AudioDevice() = default;

		/// @brief A method that creates a new audio buffer
		/// @return The AudioBuffer that has been created
		virtual AudioBufferHandle CreateAudioBuffer() = 0;

		/// @brief A method that creates an AudioSource
		/// @return The AudioSource that provides a context to play the sound
		virtual AudioSourceHandle CreateAudioSource() = 0;

		/// @brief A method that will play an AudioSource
		/// @param source The AudioSource to play
		virtual void Play(AudioSourceHandle source) = 0;

		/// @brief A virtual method that pauses an AudioSource from playing
		/// @param source The AudioSource to pause
		virtual void Pause(AudioSourceHandle source) = 0;

		/// @brief A virtual method that stops an AudioSource from playing
		/// @param source The AudioSource to stop
		virtual void Stop(AudioSourceHandle source) = 0;

		/// @brief A virtual method that rewinds an AudioSource
		/// @param source The AudioSource to rewind
		virtual void Rewind(AudioSourceHandle source) = 0;
	};
}	 // namespace Nexus::Audio