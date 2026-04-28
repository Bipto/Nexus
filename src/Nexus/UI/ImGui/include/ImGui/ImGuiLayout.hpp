#pragma once

#include <vector>

#include <memory>

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

#include "UI/Control.hpp"
#include "UI/Layout.hpp"

#include "ImGui/ImGuiControl.hpp"

namespace Nexus::UI
{
	class ImGuiLayout : public ILayout, public ImGuiControl
	{
	  public:
		ImGuiLayout()				 = default;
		virtual ~ImGuiLayout() final = default;
		IPanel	   *CreatePanel() final;
		IMenubar   *CreateMainMenubar() final;
		IStatusBar *CreateStatusBar() final;

		void Render();
	};
}	 // namespace Nexus::UI