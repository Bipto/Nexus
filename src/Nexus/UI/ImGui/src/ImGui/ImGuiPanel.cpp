#include "ImGui/ImGuiPanel.hpp"

#include "ImGui/ImGuiMenubar.hpp"

namespace Nexus::UI
{
	IMenubar *ImGuiPanel::CreateMenubar()
	{
		return AddChild<ImGuiMenubar, IMenubar>();
	}
}	 // namespace Nexus::UI