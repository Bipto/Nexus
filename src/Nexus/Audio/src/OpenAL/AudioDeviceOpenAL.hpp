#pragma once
#include "Audio/AudioDevice.hpp"

#include <memory>

#include "OpenAL.hpp"

namespace Nexus::Audio
{
	struct ALCdeviceDeleter
	{
		void operator()(ALCdevice *device) const
		{
			if (device != nullptr)
			{
				alcCloseDevice(device);
				device = nullptr;
			}
		}
	};

	struct ALCcontextDeleter
	{
		void operator()(ALCcontext *context) const
		{
			if (context != nullptr)
			{
				alcDestroyContext(context);
				context = nullptr;
			}
		}
	};

	class AudioDeviceOpenAL final : public AudioDevice
	{
	  public:
		AudioDeviceOpenAL();
		~AudioDeviceOpenAL() final = default;
		std::shared_ptr<AudioBuffer> CreateAudioBuffer() final;
		std::shared_ptr<AudioSource> CreateAudioSource() final;
		void						 Play(std::shared_ptr<AudioSource> source) final;
		void						 Pause(std::shared_ptr<AudioSource> source) final;
		void						 Stop(std::shared_ptr<AudioSource> source) final;
		void						 Rewind(std::shared_ptr<AudioSource> source) final;

	  private:
		std::unique_ptr<ALCdevice, ALCdeviceDeleter>   m_Device	 = nullptr;
		std::unique_ptr<ALCcontext, ALCcontextDeleter> m_Context = nullptr;
	};
}	 // namespace Nexus::Audio