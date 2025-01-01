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
		}

		throw std::runtime_error("Failed to find a valid audio format");
	}

	Ref<AudioBuffer> AudioDeviceOpenAL::CreateAudioBufferFromWavFile(const std::string &filepath)
	{
		nqr::WavDecoder decoder;

		nqr::AudioData data;
		decoder.LoadFromPath(&data, filepath);

		auto bitsPerSample = nqr::GetFormatBitsPerSample(data.sourceFormat);
		auto fileSize	   = data.samples.size() * sizeof(float);
		auto sampleRate	   = data.sampleRate;
		auto format		   = GetOpenALAudioFormat(data.sourceFormat, data.channelCount);
		auto dataPtr	   = (ALvoid *)data.samples.data();

		return CreateRef<AudioBufferOpenAL>(fileSize, sampleRate, format, dataPtr);
	}

	Ref<AudioBuffer> AudioDeviceOpenAL::CreateAudioBufferFromMP3File(const std::string &filepath)
	{
		nqr::Mp3Decoder decoder;

		nqr::AudioData data;
		decoder.LoadFromPath(&data, filepath);

		auto bitsPerSample = nqr::GetFormatBitsPerSample(data.sourceFormat);
		auto fileSize	   = data.samples.size() * sizeof(float);
		auto sampleRate	   = data.sampleRate;
		auto format		   = GetOpenALAudioFormat(data.sourceFormat, data.channelCount);
		auto dataPtr	   = (ALvoid *)data.samples.data();

		return CreateRef<AudioBufferOpenAL>(fileSize, sampleRate, format, dataPtr);
	}

	Ref<Audio::AudioSource> AudioDeviceOpenAL::CreateAudioSource(Ref<Audio::AudioBuffer> buffer)
	{
		return CreateRef<AudioSourceOpenAL>(buffer);
	}

	void AudioDeviceOpenAL::PlaySource(Ref<Audio::AudioSource> source)
	{
		Ref<AudioSourceOpenAL> s = std::dynamic_pointer_cast<Audio::AudioSourceOpenAL>(source);
		alSourcePlay(s->GetSource());
	}
}	 // namespace Nexus::Audio

#endif