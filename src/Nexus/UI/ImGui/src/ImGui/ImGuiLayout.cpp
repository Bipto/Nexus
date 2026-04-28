#include "ImGui/ImGuiLayout.hpp"
#include "ImGui/ImGuiMenubar.hpp"
#include "ImGui/ImGuiPanel.hpp"

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

	void ImGuiLayout::Render()
	{
		for (auto &child : m_Children) { child->Render(); }
	}
}	 // namespace Nexus::UI