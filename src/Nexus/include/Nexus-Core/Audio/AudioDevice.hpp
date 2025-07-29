#pragma once

#include "AudioBuffer.hpp"
#include "AudioSource.hpp"
#include "Nexus-Core/ApplicationSpecification.hpp"
#include "Nexus-Core/Types.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Audio
{
	/// @brief A class representing an abstraction over a graphics API and context
	class NX_API AudioDevice
	{
	  public:
		/// @brief Virtual destructor used to destroy any necessary data
		virtual ~AudioDevice()
		{
		}

		/// @brief A method providing the currently used audio backend
		/// @return An AudioAPI enum representing the current backend
		virtual AudioAPI GetAPI() = 0;

		/// @brief A method that creates a new audio buffer
		/// @return The AudioBuffer that has been created
		virtual Ref<AudioBuffer> CreateAudioBuffer() = 0;

		/// @brief A method that creates an AudioSource
		/// @return The AudioSource that provides a context to play the sound
		virtual Ref<AudioSource> CreateAudioSource() = 0;

		/// @brief A method that will play an AudioSource
		/// @param source The AudioSource to play
		virtual void Play(Ref<AudioSource> source) = 0;

		/// @brief A virtual method that pauses an AudioSource from playing
		/// @param source The AudioSource to pause
		virtual void Pause(Ref<AudioSource> source) = 0;

		/// @brief A virtual method that stops an AudioSource from playing
		/// @param source The AudioSource to stop
		virtual void Stop(Ref<AudioSource> source) = 0;

		/// @brief A virtual method that rewinds an AudioSource
		/// @param source The AudioSource to rewind
		virtual void Rewind(Ref<AudioSource> source) = 0;
	};
}	 // namespace Nexus::Audio