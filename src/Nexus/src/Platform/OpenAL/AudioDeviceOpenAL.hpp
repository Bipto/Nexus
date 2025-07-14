#pragma once

#if defined(NX_PLATFORM_OPENAL)

	#include "Nexus-Core/Audio/AudioDevice.hpp"
	#include "Nexus-Core/nxpch.hpp"
	#include "OpenAL.hpp"

namespace Nexus::Audio
{
	class AudioDeviceOpenAL : public AudioDevice
	{
	  public:
		AudioDeviceOpenAL();
		virtual ~AudioDeviceOpenAL();
		virtual AudioAPI GetAPI() override
		{
			return AudioAPI::OpenAL;
		}
		Ref<AudioBuffer> CreateAudioBuffer() final;
		Ref<AudioSource> CreateAudioSource() final;
		void			 Play(Ref<AudioSource> source) final;
		void			 Pause(Ref<AudioSource> source) final;
		void			 Stop(Ref<AudioSource> source) final;
		void			 Rewind(Ref<AudioSource> source) final;

	  private:
		ALCdevice  *m_Device  = nullptr;
		ALCcontext *m_Context = nullptr;
	};
}	 // namespace Nexus::Audio

#endif