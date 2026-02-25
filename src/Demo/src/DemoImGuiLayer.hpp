#pragma once

#include "Nexus-Core/ImGui/ImGuiLayer.hpp"

class DemoImGuiLayer : public Nexus::ImGuiLayer
{
  public:
	DemoImGuiLayer(Nexus::Application *app, Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue) : Nexus::ImGuiLayer(app, commandQueue)
	{
		ImGuiContext *context = m_ImGuiRenderer->GetContext();
		ImGui::SetCurrentContext(context);
	}

	virtual ~DemoImGuiLayer() = default;
	void OnImGuiRenderer() final
	{
		ImGui::ShowDemoWindow();
	}
};