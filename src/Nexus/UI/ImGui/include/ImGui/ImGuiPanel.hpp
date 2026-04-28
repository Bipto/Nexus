#pragma once

#include <memory>
#include <string>
#include <vector>

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

#include "UI/Control.hpp"
#include "UI/Menubar.hpp"
#include "UI/Panel.hpp"

#include "ImGui/ImGuiControl.hpp"

namespace Nexus::UI
{
	class ImGuiPanel : public IPanel, public ImGuiControl
	{
	  public:
		ImGuiPanel()		  = default;
		virtual ~ImGuiPanel() = default;
		IMenubar *CreateMenubar() final;
		void	  Render() final;
	};
}	 // namespace Nexus::UI