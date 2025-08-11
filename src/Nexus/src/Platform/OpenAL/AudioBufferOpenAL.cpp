#include "AudioBufferOpenAL.hpp"

#if defined(NX_PLATFORM_OPENAL)

	#include "Nexus-Core/nxpch.hpp"

void CheckError()
{
	ALenum error = alGetError();
	if (error != AL_NO_ERROR)
	{
		auto message = alGetString(error);
		std::cout << message << std::endl;
	}
}

namespace Nexus::Audio
{
	AudioBufferOpenAL::AudioBufferOpenAL(AudioDeviceOpenAL *device) : m_Device(device)
	{
		alGenBuffers(1, &m_Buffer);
		CheckError();
	}

	AudioBufferOpenAL::~AudioBufferOpenAL()
	{
		alDeleteBuffers(1, &m_Buffer);
	}

	const ALuint AudioBufferOpenAL::GetHandle() const
	{
		return m_Buffer;
	}

	void AudioBufferOpenAL::SetData(const void *const data, size_t size, AudioFormat format, size_t frequency)
	{
		ALenum alFormat = AL::GetOpenALFormat(format);
		alBufferData(m_Buffer, alFormat, data, size, frequency);
		CheckError();
	}

	size_t AudioBufferOpenAL::GetFrequency() const
	{
		ALint frequency;
		alGetBufferi(m_Buffer, AL_FREQUENCY, &frequency);
		return static_cast<size_t>(frequency);
	}

	size_t AudioBufferOpenAL::GetBits() const
	{
		ALint bits;
		alGetBufferi(m_Buffer, AL_BITS, &bits);
		return static_cast<size_t>(bits);
	}

	size_t AudioBufferOpenAL::GetChannels() const
	{
		ALint channels;
		alGetBufferi(m_Buffer, AL_CHANNELS, &channels);
		return static_cast<size_t>(channels);
	}

	size_t AudioBufferOpenAL::GetSize() const
	{
		ALint size;
		alGetBufferi(m_Buffer, AL_SIZE, &size);
		return static_cast<size_t>(size);
	}
}	 // namespace Nexus::Audio

#endif