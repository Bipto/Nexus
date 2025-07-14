#include "OpenAL.hpp"

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::AL
{
	ALenum GetOpenALFormat(Audio::AudioFormat format)
	{
		switch (format)
		{
			case Audio::AudioFormat::Mono8: return AL_FORMAT_MONO8;
			case Audio::AudioFormat::Stereo8: return AL_FORMAT_STEREO8;
			case Audio::AudioFormat::Mono16: return AL_FORMAT_MONO16;
			case Audio::AudioFormat::Stereo16: return AL_FORMAT_STEREO16;
			case Audio::AudioFormat::MonoFloat32: return AL_FORMAT_MONO_FLOAT32;
			case Audio::AudioFormat::StereoFloat32: return AL_FORMAT_STEREO_FLOAT32;
			default: throw std::runtime_error("Failed to find a valid audio format");
		}
	}
	Audio::SourceType GetSourceType(ALint type)
	{
		switch (type)
		{
			case AL_UNDETERMINED: return Audio::SourceType::Undetermined;
			case AL_STATIC: return Audio::SourceType::Static;
			case AL_STREAMING: return Audio::SourceType::Streaming;
			default: throw std::runtime_error("Failed to find a valid source type");
		}
	}

	Audio::SourceState GetSourceState(ALint state)
	{
		switch (state)
		{
			case AL_INITIAL: return Audio::SourceState::Initial;
			case AL_PLAYING: return Audio::SourceState::Playing;
			case AL_PAUSED: return Audio::SourceState::Paused;
			case AL_STOPPED: return Audio::SourceState::Stopped;
			default: throw std::runtime_error("Failed to find a valid source state");
		}
	}
}	 // namespace Nexus::AL
