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

	std::shared_ptr<AudioBuffer> AudioDeviceOpenAL::CreateAudioBuffer()
	{
		return std::make_shared<AudioBufferOpenAL>(this);
	}

	std::shared_ptr<Audio::AudioSource> AudioDeviceOpenAL::CreateAudioSource()
	{
		return std::make_shared<AudioSourceOpenAL>();
	}

	void AudioDeviceOpenAL::Play(std::shared_ptr<AudioSource> source)
	{
		std::shared_ptr<AudioSourceOpenAL> alSource = std::dynamic_pointer_cast<Audio::AudioSourceOpenAL>(source);
		alSourcePlay(alSource->GetSource());
	}

	void AudioDeviceOpenAL::Pause(std::shared_ptr<AudioSource> source)
	{
		std::shared_ptr<AudioSourceOpenAL> alSource = std::dynamic_pointer_cast<Audio::AudioSourceOpenAL>(source);
		alSourcePause(alSource->GetSource());
	}

	void AudioDeviceOpenAL::Stop(std::shared_ptr<AudioSource> source)
	{
		std::shared_ptr<AudioSourceOpenAL> alSource = std::dynamic_pointer_cast<Audio::AudioSourceOpenAL>(source);
		alSourceStop(alSource->GetSource());
	}

	void AudioDeviceOpenAL::Rewind(std::shared_ptr<AudioSource> source)
	{
		std::shared_ptr<AudioSourceOpenAL> alSource = std::dynamic_pointer_cast<Audio::AudioSourceOpenAL>(source);
		alSourceRewind(alSource->GetSource());
	}
}	 // namespace Nexus::Audio