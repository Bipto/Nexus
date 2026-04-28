#include "ImGui/ImGuiLayout.hpp"
#include "ImGui/ImGuiMenubar.hpp"
#include "ImGui/ImGuiPanel.hpp"
#include "ImGui/ImGuiStatusBar.hpp"

namespace Nexus::UI
{
	IPanel *ImGuiLayout::CreatePanel()
	{
		return AddChild<ImGuiPanel, IPanel>();
	}

	IMenubar *ImGuiLayout::CreateMainMenubar()
	{
		return AddChild<ImGuiMenubar, IMenubar>(true);
	}

	IStatusBar *ImGuiLayout::CreateStatusBar()
	{
		return AddChild<ImGuiStatusBar, IStatusBar>();
	}

	void ImGuiLayout::Render()
	{
		for (auto &child : m_Children) { child->Render(); }
	}
}	 // namespace Nexus::UI