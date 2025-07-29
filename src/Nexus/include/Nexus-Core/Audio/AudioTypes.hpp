#pragma once

namespace Nexus::Audio
{
	/// @brief An enum class representing the format of audio data within a buffer
	enum class AudioFormat
	{
		/// @brief Each sample of data is comprised of a 8 bit integer and has a single channel
		Mono8,

		/// @brief Each sample of data is comprised of a 8 bit integer and has 2 channels
		Stereo8,

		/// @brief Each sample of data is comprised of a 16 bit integer and has a single channel
		Mono16,

		/// @brief Each sample of data is comprised of a 16 bit integer and has 2 channels
		Stereo16,

		/// @brief Each sample of data is comprised of a 32 bit float and has a single channel
		MonoFloat32,

		/// @brief Each sample of data is comprised of a 32 bit float and has 2 channels
		StereoFloat32
	};

	/// @brief An enum class representing the type of audio source
	enum class SourceType
	{
		/// @brief The type of source is unknown
		Undetermined,

		/// @brief The source has a single static audio buffer to play
		Static,

		/// @brief The source has a queue of audio buffers to play
		Streaming
	};

	/// @brief An enum class representing the current state of the audio source
	enum class SourceState
	{
		/// @brief The audio source is currently being played
		Playing,

		/// @brief The audio source is currently paused
		Paused,

		/// @brief The audio source has been stopped
		Stopped,

		/// @brief The audio source is at the beginning and has either not been played yet or has been rewound
		Initial
	};
}	 // namespace Nexus::Audio