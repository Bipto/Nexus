#pragma once

#include <memory>
#include <string>
#include <vector>

#include <wx/aui/aui.h>
#include <wx/wx.h>

#include "UI/Control.hpp"
#include "UI/Menubar.hpp"
#include "UI/Panel.hpp"

#include "wxWidgets/wxWidgetsBaseControl.hpp"

namespace Nexus::UI
{
	class wxWidgetsPanel : public IPanel, public wxWidgetsBaseControl
	{
	  public:
		wxWidgetsPanel(wxFrame *frame);
		virtual ~wxWidgetsPanel() = default;
		IMenubar *CreateMenubar() final;

		wxPanel *GetPanel();

	  private:
		wxFrame *m_Frame = nullptr;
		wxPanel *m_Panel = nullptr;
	};
}	 // namespace Nexus::UI