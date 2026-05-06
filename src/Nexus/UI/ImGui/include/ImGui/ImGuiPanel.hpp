#pragma once

#include <memory>
#include <optional>
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
	class ImGuiPanel final : public IPanel, public ImGuiControl
	{
	  public:
		ImGuiPanel()		= default;
		~ImGuiPanel() final = default;
		IButton *CreateButton(std::string_view text) final;

		void Render() final;

	  private:
		std::string m_Title = {};
		bool		m_Open	= true;

		std::optional<Position> m_Position = {};
		std::optional<Size>		m_Size	   = {};
	};
}	 // namespace Nexus::UI