#pragma once

#include <vector>

#include <memory>

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

#include "UI/Control.hpp"
#include "UI/Layout.hpp"

namespace Nexus::UI
{
	class ImGuiLayout : public ILayout
	{
	  public:
		ImGuiLayout()		   = default;
		virtual ~ImGuiLayout() = default;
		IPanel	 *CreatePanel() final;
		IMenubar *CreateMainMenubar() final;
	};
}	 // namespace Nexus::UI