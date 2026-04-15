#include "AudioDeviceOpenAL.hpp"

#include "AudioBufferOpenAL.hpp"
#include "AudioSourceOpenAL.hpp"
#include "OpenAL.hpp"

#include <stdexcept>
#include <string>

namespace Nexus::Audio
{
	AudioDeviceOpenAL::AudioDeviceOpenAL()
	{
		std::string deviceName = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
		m_Device.reset(alcOpenDevice(deviceName.c_str()));

		if (!m_Device)
		{
			throw std::runtime_error("Failed to create audio device");
		}

		m_Context.reset(alcCreateContext(m_Device.get(), nullptr));
		alcMakeContextCurrent(m_Context.get());
	}

	AudioBufferHandle AudioDeviceOpenAL::CreateAudioBuffer()
	{
		auto buffer = std::make_unique<AudioBufferOpenAL>(this);
		return m_Resources.AudioBuffers.CreateShared(std::move(buffer));
	}

	AudioSourceHandle AudioDeviceOpenAL::CreateAudioSource()
	{
		auto source = std::make_unique<AudioSourceOpenAL>();
		return m_Resources.AudioSources.CreateShared(std::move(source));
	}

	void AudioDeviceOpenAL::Play(AudioSourceHandle source)
	{
		if (const AudioSourceOpenAL *alSource = source.AsDerived<AudioSourceOpenAL>())
		{
			alSourcePlay(alSource->GetSource());
		}
	}

	void AudioDeviceOpenAL::Pause(AudioSourceHandle source)
	{
		if (const AudioSourceOpenAL *alSource = source.AsDerived<AudioSourceOpenAL>())
		{
			alSourcePause(alSource->GetSource());
		}
	}

	void AudioDeviceOpenAL::Stop(AudioSourceHandle source)
	{
		if (const AudioSourceOpenAL *alSource = source.AsDerived<AudioSourceOpenAL>())
		{
			alSourceStop(alSource->GetSource());
		}
	}

	void AudioDeviceOpenAL::Rewind(AudioSourceHandle source)
	{
		if (const AudioSourceOpenAL *alSource = source.AsDerived<AudioSourceOpenAL>())
		{
			alSourceRewind(alSource->GetSource());
		}
	}
}	 // namespace Nexus::Audio