#pragma once

#include "Demo.hpp"

namespace Demos
{
	class FramebufferDemo : public Demo
	{
	  public:
		FramebufferDemo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
			: Demo(name, app, imGuiRenderer)
		{
		}

		virtual ~FramebufferDemo()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_GraphicsDevice->CreateCommandList();
			Nexus::Graphics::FramebufferSpecification spec;
			spec.Width						  = 1280;
			spec.Height						  = 720;
			spec.ColorAttachmentSpecification = {Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm};
			spec.Samples					  = Nexus::Graphics::SampleCount::SampleCount1;

			m_Framebuffer = m_GraphicsDevice->CreateFramebuffer(spec);

			auto texture = m_Framebuffer->GetColorTexture(0);
			m_TextureID	 = m_ImGuiRenderer->BindTexture(texture);
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
			m_CommandList->Begin();
			m_CommandList->SetRenderTarget(Nexus::Graphics::RenderTarget {m_Framebuffer});
			m_CommandList->ClearColorTarget(0, {m_RenderTargetClearColour.r, m_RenderTargetClearColour.g, m_RenderTargetClearColour.b, 1.0f});
			m_CommandList->End();
			m_GraphicsDevice->SubmitCommandList(m_CommandList);

			m_CommandList->Begin();
			m_CommandList->SetRenderTarget(Nexus::Graphics::RenderTarget {m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()});
			m_CommandList->ClearColorTarget(0, {m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, 1.0f});
			m_CommandList->End();
			m_GraphicsDevice->SubmitCommandList(m_CommandList);
		}

		virtual void RenderUI() override
		{
			ImGui::ColorEdit3("Clear Color", glm::value_ptr(m_RenderTargetClearColour));
			ImGui::Image(m_TextureID, {256, 256});
		}

		virtual std::string GetInfo() const override
		{
			return "Rendering into a framebuffer and displaying the texture onto the screen";
		}

	  private:
		Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
		glm::vec3								 m_ClearColour = {100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f};

		Nexus::Ref<Nexus::Graphics::Framebuffer> m_Framebuffer			   = nullptr;
		ImTextureID								 m_TextureID			   = 0;
		glm::vec3								 m_RenderTargetClearColour = {0.75f, 0.35f, 0.42f};
	};
}	 // namespace Demos