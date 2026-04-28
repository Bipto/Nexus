#include "ImGui/ImGuiPanel.hpp"

#include "ImGui/ImGuiMenubar.hpp"

namespace Nexus::UI
{
	IMenubar *ImGuiPanel::CreateMenubar()
	{
		return AddChild<ImGuiMenubar, IMenubar>(false);
	}

	void ImGuiPanel::Render()
	{
	}
}	 // namespace Nexus::UI