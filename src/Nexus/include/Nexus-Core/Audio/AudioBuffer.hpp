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

		virtual void   SetData(const void *const data, size_t size, AudioFormat format, size_t frequency) = 0;
		virtual size_t GetFrequency() const																  = 0;
		virtual size_t GetBits() const																	  = 0;
		virtual size_t GetChannels() const																  = 0;
		virtual size_t GetSize() const																	  = 0;
	};
}	 // namespace Nexus::Audio