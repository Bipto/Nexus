#pragma once

#include "AudioBuffer.hpp"
#include "Nexus-Core/Types.hpp"

namespace Nexus::Audio
{
	/// @brief A pure virtual class representing a buffer of audio data
	class AudioBuffer
	{
	  public:
		/// @brief A virtual destructor to allow resources to be cleaned up
		NX_API virtual ~AudioBuffer()
		{
		}
	};
}	 // namespace Nexus::Audio