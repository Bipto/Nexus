#include "AudioDeviceOpenAL.hpp"

#if defined(NX_PLATFORM_OPENAL)

	#include "AudioBufferOpenAL.hpp"
	#include "AudioSourceOpenAL.hpp"
	#include "OpenAL.hpp"
	#include "libnyquist/Common.h"
	#include "libnyquist/Decoders.h"

namespace Nexus::Audio
{
	AudioDeviceOpenAL::AudioDeviceOpenAL()
	{
		std::string deviceName = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
		m_Device			   = alcOpenDevice(deviceName.c_str());
		if (!m_Device)
		{
			throw std::runtime_error("Failed to create audio device");
		}

		m_Context = alcCreateContext(m_Device, nullptr);
		alcMakeContextCurrent(m_Context);
	}

	AudioDeviceOpenAL::~AudioDeviceOpenAL()
	{
		alcDestroyContext(m_Context);
		alcCloseDevice(m_Device);
	}

	ALenum GetOpenALAudioFormat(nqr::PCMFormat format, int channelCount)
	{
		bool stereo = channelCount > 1;
		switch (format)
		{
			case nqr::PCMFormat::PCM_U8:
			case nqr::PCMFormat::PCM_S8:
			{
				if (stereo)
				{
					return AL_FORMAT_STEREO8;
				}
				else
				{
					return AL_FORMAT_MONO8;
				}
			}
			case nqr::PCMFormat::PCM_16:
			case nqr::PCMFormat::PCM_24:
			{
				if (stereo)
				{
					return AL_FORMAT_STEREO16;
				}
				else
				{
					return AL_FORMAT_MONO16;
				}
			}
			case nqr::PCMFormat::PCM_32:
			case nqr::PCMFormat::PCM_64:
			case nqr::PCMFormat::PCM_FLT:
			case nqr::PCMFormat::PCM_DBL:
			{
				if (stereo)
				{
					return AL_FORMAT_STEREO_FLOAT32;
				}
				else
				{
					return AL_FORMAT_MONO_FLOAT32;
				}
			}
			case nqr::PCMFormat::PCM_END: throw std::runtime_error("Invalid audio format");
			default: throw std::runtime_error("Failed to find a valid audio format");
		}
	}

	Ref<AudioBuffer> AudioDeviceOpenAL::CreateAudioBuffer()
	{
		return CreateRef<AudioBufferOpenAL>(this);
	}

	Ref<Audio::AudioSource> AudioDeviceOpenAL::CreateAudioSource()
	{
		return CreateRef<AudioSourceOpenAL>();
	}

	void AudioDeviceOpenAL::Play(Ref<AudioSource> source)
	{
		Ref<AudioSourceOpenAL> alSource = std::dynamic_pointer_cast<Audio::AudioSourceOpenAL>(source);
		alSourcePlay(alSource->GetSource());
	}

	void AudioDeviceOpenAL::Pause(Ref<AudioSource> source)
	{
		Ref<AudioSourceOpenAL> alSource = std::dynamic_pointer_cast<Audio::AudioSourceOpenAL>(source);
		alSourcePause(alSource->GetSource());
	}

	void AudioDeviceOpenAL::Stop(Ref<AudioSource> source)
	{
		Ref<AudioSourceOpenAL> alSource = std::dynamic_pointer_cast<Audio::AudioSourceOpenAL>(source);
		alSourceStop(alSource->GetSource());
	}

	void AudioDeviceOpenAL::Rewind(Ref<AudioSource> source)
	{
		Ref<AudioSourceOpenAL> alSource = std::dynamic_pointer_cast<Audio::AudioSourceOpenAL>(source);
		alSourceRewind(alSource->GetSource());
	}
}	 // namespace Nexus::Audio

#endif