#pragma once

#include "Demo.hpp"

namespace Demos
{
	class AudioDemo : public Demo
	{
	  public:
		AudioDemo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
			: Demo(name, app, imGuiRenderer)
		{
		}

		virtual ~AudioDemo()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_GraphicsDevice->CreateCommandList();
			m_AudioBuffer =
			m_AudioDevice->CreateAudioBufferFromWavFile({Nexus::FileSystem::GetFilePathAbsolute("resources/demo/audio/laser_shoot.wav")});
			m_AudioSource = m_AudioDevice->CreateAudioSource(m_AudioBuffer);
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
			m_CommandList->Begin();
			m_CommandList->SetRenderTarget(Nexus::Graphics::RenderTarget {m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()});
			m_CommandList->ClearColorTarget(0, {m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, 1.0f});
			m_CommandList->End();
			m_GraphicsDevice->SubmitCommandList(m_CommandList);
		}

		virtual void RenderUI() override
		{
			ImGui::Text("Press button to play a sound effect");
			if (ImGui::Button("Play"))
			{
				m_AudioDevice->PlaySource(m_AudioSource);
			}
		}

	  private:
		Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
		glm::vec3								 m_ClearColour = {100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f};

		Nexus::Ref<Nexus::Audio::AudioBuffer> m_AudioBuffer;
		Nexus::Ref<Nexus::Audio::AudioSource> m_AudioSource;
	};
}	 // namespace Demos