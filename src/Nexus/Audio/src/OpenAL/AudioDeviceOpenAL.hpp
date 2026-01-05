#pragma once
#include "Audio/AudioDevice.hpp"

#include <memory>

#include "OpenAL.hpp"

namespace Nexus::Audio
{
	class AudioDeviceOpenAL : public AudioDevice
	{
	  public:
		AudioDeviceOpenAL();
		virtual ~AudioDeviceOpenAL();
		std::shared_ptr<AudioBuffer> CreateAudioBuffer() final;
		std::shared_ptr<AudioSource> CreateAudioSource() final;
		void						 Play(std::shared_ptr<AudioSource> source) final;
		void						 Pause(std::shared_ptr<AudioSource> source) final;
		void						 Stop(std::shared_ptr<AudioSource> source) final;
		void						 Rewind(std::shared_ptr<AudioSource> source) final;

	  private:
		ALCdevice  *m_Device  = nullptr;
		ALCcontext *m_Context = nullptr;
	};
}	 // namespace Nexus::Audio