#pragma once

#include <memory>

#include <wx/aui/aui.h>
#include <wx/wx.h>

#include "UI/Layout.hpp"

namespace Nexus::UI
{
	class wxWidgetsLayout : ILayout
	{
	  public:
		wxWidgetsLayout(wxFrame *frame);
		virtual ~wxWidgetsLayout() = default;
		IPanel *CreatePanel() final;

	  private:
		wxFrame *m_Frame = nullptr;
	};
}	 // namespace Nexus::UI