#pragma once

#include <vector>

#include <memory>

#include <wx/aui/aui.h>
#include <wx/wx.h>

#include "UI/Control.hpp"
#include "UI/Layout.hpp"

#include "wxWidgets/wxWidgetsBaseControl.hpp"

namespace Nexus::UI
{
	class wxWidgetsLayout final : public ILayout, public wxWidgetsBaseControl
	{
	  public:
		wxWidgetsLayout(wxFrame *frame);
		~wxWidgetsLayout() final = default;
		IPanel	   *CreatePanel() final;
		IMenubar   *CreateMainMenubar() final;
		IStatusBar *CreateStatusBar() final;

	  private:
		wxFrame *m_Frame = nullptr;
	};
}	 // namespace Nexus::UI