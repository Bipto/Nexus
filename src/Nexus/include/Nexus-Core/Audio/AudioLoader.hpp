#pragma once

#include "AudioDevice.hpp"

namespace Nexus::Audio
{
	class NX_API AudioLoader
	{
	  public:
		static Ref<AudioBuffer> LoadWavFile(const std::string &filepath, AudioDevice *device);
		static Ref<AudioBuffer> LoadMp3File(const std::string &filepath, AudioDevice *device);
	};
};	  // namespace Nexus::Audio