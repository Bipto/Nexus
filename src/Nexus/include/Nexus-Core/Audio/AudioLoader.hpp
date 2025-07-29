#pragma once

#include "AudioDevice.hpp"

namespace Nexus::Audio
{
	/// @brief A class that is used to load audio files into an audio buffer
	class NX_API AudioLoader
	{
	  public:
		/// @brief A method that loads a WAV file from disk and generates an audio buffer from it
		/// @param filepath The path to the WAV file to load
		/// @param device The audio device to use to create the audio buffer
		/// @return A created audio buffer containing the data loaded from the file
		static Ref<AudioBuffer> LoadWavFile(const std::string &filepath, AudioDevice *device);

		/// @brief A method that loads a MP3 file from disk and generates an audio buffer from it
		/// @param filepath The path to the MP3 file to load
		/// @param device The audio device to use to create the audio buffer
		/// @return A created audio buffer containing the data loaded from the file
		static Ref<AudioBuffer> LoadMp3File(const std::string &filepath, AudioDevice *device);
	};
};	  // namespace Nexus::Audio