#pragma once

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/ImGui/ImGuiGraphicsRenderer.hpp"
#include "Panel.hpp"
#include "glm/glm.hpp"

namespace Editor
{
	class SceneViewport : public Panel
	{
	  public:
		SceneViewport() = delete;
		SceneViewport(Nexus::Graphics::GraphicsDevice *graphicsDevice, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiGraphicsRenderer);
		virtual void OnLoad() override;
		virtual void OnRender() override;

	  private:
		void ResizeViewport(uint32_t width, uint32_t height);
		void RenderScene();

	  private:
		uint32_t m_CurrentWidth	 = 0;
		uint32_t m_CurrentHeight = 0;

		Nexus::Graphics::GraphicsDevice			 *m_GraphicsDevice		  = nullptr;
		Nexus::ImGuiUtils::ImGuiGraphicsRenderer *m_ImGuiGraphicsRenderer = nullptr;
		Nexus::Ref<Nexus::Graphics::Framebuffer>  m_ViewportFramebuffer	  = nullptr;
		Nexus::Ref<Nexus::Graphics::CommandList>  m_CommandList			  = nullptr;

		ImTextureID m_ViewportTextureID = 0;
	};
}	 // namespace Editor