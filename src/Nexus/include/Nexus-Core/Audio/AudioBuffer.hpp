#pragma once

#include "AudioBuffer.hpp"
#include "Nexus-Core/Types.hpp"

#include "AudioTypes.hpp"

namespace Nexus::Audio
{
	/// @brief A pure virtual class representing a buffer of audio data
	class NX_API AudioBuffer
	{
	  public:
		/// @brief A virtual destructor to allow resources to be cleaned up
		virtual ~AudioBuffer()
		{
		}

		/// @brief A virtual method that sets the data of an audio buffer, the size of the data does not have to match the previous size of the
		/// buffer, it is able to dynamically resize
		/// @param data A pointer to the data to upload to the buffer
		/// @param size The size of the data in bytes
		/// @param format The format of the data, including the size in bytes and whether it is mono or stereo
		/// @param frequency The sample frequency of the data
		virtual void   SetData(const void *const data, size_t size, AudioFormat format, size_t frequency) = 0;

		/// @brief A virtual method that returns the frequency of the sampled data
		/// @return The frequency of the data
		virtual size_t GetFrequency() const																  = 0;

		/// @brief A virtual method that returns the bit depth of the buffer
		/// @return The bit depth of the buffer
		virtual size_t GetBits() const																	  = 0;

		/// @brief A virtual method that returns the channel count of the buffer
		/// @return The number of channels in the buffer
		virtual size_t GetChannels() const																  = 0;

		/// @brief A virtual method that returns the size of the buffer in bytes
		/// @return The size of the buffer in bytes
		virtual size_t GetSize() const																	  = 0;
	};
}	 // namespace Nexus::Audio