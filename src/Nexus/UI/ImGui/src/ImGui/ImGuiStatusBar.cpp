#include "ImGui/ImGuiStatusBar.hpp"

#include "UI/StatusBar.hpp"

namespace Nexus::UI
{
	void ImGuiStatusBar::SetStatusText(std::string_view text)
	{
		m_StatusText = std::string {text};
	}

	void ImGuiStatusBar::SetHelpText(std::string_view text)
	{
		m_HelpText = text;
	}

	static void HelpMarker(const std::string &desc)
	{
		if (desc.empty())
		{
			return;
		}

		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("%s", desc);
	}

	void ImGuiStatusBar::Render()
	{
		ImGuiViewport *vp		 = ImGui::GetMainViewport();
		float		   barHeight = ImGui::GetFrameHeight() + ImGui::GetStyle().FramePadding.y * 2.0f;

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoInputs;

		if (ImGui::BeginViewportSideBar("StatusBar", vp, ImGuiDir_Down, barHeight, flags))
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::Text(m_StatusText.c_str());
				HelpMarker(m_HelpText);
				ImGui::EndMenuBar();
			}
			ImGui::End();
		}
	}
}	 // namespace Nexus::UI