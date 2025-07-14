#pragma once

namespace Nexus::Audio
{
	enum class AudioFormat
	{
		Mono8,
		Stereo8,
		Mono16,
		Stereo16,
		MonoFloat32,
		StereoFloat32
	};

	enum class SourceType
	{
		Undetermined,
		Static,
		Streaming
	};

	enum class SourceState
	{
		Playing,
		Paused,
		Stopped,
		Initial
	};
}	 // namespace Nexus::Audio