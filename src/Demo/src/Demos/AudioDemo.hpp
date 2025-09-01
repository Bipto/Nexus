#pragma once

#include "Demo.hpp"

#include "Nexus-Core/Audio/AudioLoader.hpp"

namespace Demos
{
	class AudioDemo : public Demo
	{
	  public:
		AudioDemo(const std::string							&name,
				  Nexus::Application						*app,
				  Nexus::ImGuiUtils::ImGuiGraphicsRenderer	*imGuiRenderer,
				  Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue)
			: Demo(name, app, imGuiRenderer, commandQueue)
		{
		}

		virtual ~AudioDemo()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_GraphicsDevice->CreateCommandList();
			m_AudioBuffer = Nexus::Audio::AudioLoader::LoadWavFile({Nexus::FileSystem::GetFilePathAbsolute("resources/demo/audio/laser_shoot.wav")},
																   m_AudioDevice);
			m_AudioSource = m_AudioDevice->CreateAudioSource();
			m_AudioSource->SetStaticSourceBuffer(m_AudioBuffer);
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
			m_CommandList->Begin();
			m_CommandList->SetRenderTarget(Nexus::Graphics::RenderTarget {Nexus::GetApplication()->GetPrimarySwapchain()});
			m_CommandList->ClearColorTarget(0, {m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, 1.0f});
			m_CommandList->End();

			m_CommandQueue->SubmitCommandLists(&m_CommandList, 1, nullptr);
			m_GraphicsDevice->WaitForIdle();
		}

		virtual void RenderUI() override
		{
			ImGui::Text("Press button to play a sound effect");
			if (ImGui::Button("Play"))
			{
				m_AudioDevice->Play(m_AudioSource);
			}

			if (ImGui::DragFloat("Gain", &m_Gain, 0.01f, 0.0f, 1.0f))
			{
				m_AudioSource->SetGain(m_Gain);
			}

			if (ImGui::Checkbox("Loop", &m_IsLooping))
			{
				m_AudioSource->SetIsLooping(m_IsLooping);
			}
		}

		virtual std::string GetInfo() const override
		{
			return "Playing a sound effect loaded from a .wav file";
		}

	  private:
		Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
		glm::vec3								 m_ClearColour = {100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f};

		Nexus::Ref<Nexus::Audio::AudioBuffer> m_AudioBuffer;
		Nexus::Ref<Nexus::Audio::AudioSource> m_AudioSource;

		float m_Gain	  = 1.0f;
		bool  m_IsLooping = false;
	};
}	 // namespace Demos