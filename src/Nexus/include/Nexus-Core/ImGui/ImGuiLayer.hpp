#pragma once

#include "Nexus-Core/ImGui/ImGuiGraphicsRenderer.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "Platform/Layers/Layer.hpp"

namespace Nexus
{
	class NX_API ImGuiLayer : public ILayer
	{
	  public:
		ImGuiLayer(Nexus::Application *app, Nexus::Ref<Graphics::ICommandQueue> commandQueue);
		virtual ~ImGuiLayer() = default;
		bool		 OnEvent(const Event &event) final;
		void		 OnRender(Nexus::TimeSpan time, IWindow *window) final;
		void		 OnUpdate(Nexus::TimeSpan time, IWindow *window) final;
		void		 OnTick(Nexus::TimeSpan time, IWindow *window) final;
		virtual void OnImGuiRenderer() = 0;

	  protected:
		std::unique_ptr<ImGuiUtils::ImGuiGraphicsRenderer> m_ImGuiRenderer		= nullptr;
		Nexus::Ref<Graphics::ICommandQueue>				   m_CommandQueue		= nullptr;
		Nexus::Ref<Graphics::ICommandList>				   m_CommandList		= nullptr;
		bool											   m_IsAnyWindowHovered = false;
	};
}	 // namespace Nexus