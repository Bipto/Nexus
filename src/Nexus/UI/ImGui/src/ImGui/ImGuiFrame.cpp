#include "ImGui/ImGuiFrame.hpp"
#include "ImGui/ImGuiMenubar.hpp"
#include "ImGui/ImGuiStatusbar.hpp"

namespace Nexus::UI
{
	ImGuiFrame::ImGuiFrame(const std::string &title) : m_Title(title)
	{
	}

	IMenubar *ImGuiFrame::CreateMenubar()
	{
		m_Menubar = std::make_unique<ImGuiMenubar>(true);
		return m_Menubar.get();
	}

	IStatusBar *ImGuiFrame::CreateStatusbar()
	{
		m_StatusBar = std::make_unique<ImGuiStatusBar>();
		return m_StatusBar.get();
	}

	void ImGuiFrame::Render()
	{
		if (m_Menubar)
		{
			m_Menubar->Render();
		}

		if (m_StatusBar)
		{
			m_StatusBar->Render();
		}
	}
}	 // namespace Nexus::UI
