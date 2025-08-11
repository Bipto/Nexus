#pragma once

#if defined(NX_PLATFORM_OPENAL)

	#include "Nexus-Core/Audio/AudioBuffer.hpp"
	#include "OpenAL.hpp"

namespace Nexus::Audio
{
	class AudioDeviceOpenAL;

	class AudioBufferOpenAL : public AudioBuffer
	{
	  public:
		AudioBufferOpenAL(AudioDeviceOpenAL *device);
		virtual ~AudioBufferOpenAL();
		const ALuint   GetHandle() const;
		virtual void   SetData(const void *const data, size_t size, AudioFormat format, size_t frequency) override;
		virtual size_t GetFrequency() const override;
		virtual size_t GetBits() const override;
		virtual size_t GetChannels() const override;
		virtual size_t GetSize() const override;

	  private:
		AudioDeviceOpenAL *m_Device = nullptr;
		ALuint			   m_Buffer = 0;
	};
}	 // namespace Nexus::Audio
#endif