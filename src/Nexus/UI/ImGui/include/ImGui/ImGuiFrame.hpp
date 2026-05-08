#pragma once

#include "UI/Frame.hpp"
#include "UI/Menubar.hpp"
#include "UI/StatusBar.hpp"

#include "ImGui/ImGuiControl.hpp"
#include "ImGui/ImGuiMenubar.hpp"
#include "ImGui/ImGuiStatusBar.hpp"

namespace Nexus::UI
{
	class ImGuiFrame final : public IFrame, public ImGuiControl
	{
	  public:
		ImGuiFrame(const std::string &title);
		~ImGuiFrame() final = default;
		IMenubar   *CreateMenubar() final;
		IStatusBar *CreateStatusbar() final;

		void Render() final;

	  private:
		std::string						m_Title		= {};
		std::unique_ptr<ImGuiMenubar>	m_Menubar	= nullptr;
		std::unique_ptr<ImGuiStatusBar> m_StatusBar = nullptr;
	};
}	 // namespace Nexus::UI