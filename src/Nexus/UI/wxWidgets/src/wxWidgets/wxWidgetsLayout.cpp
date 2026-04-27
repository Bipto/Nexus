#include "wxWidgets/wxWidgetsLayout.hpp"

#include "wxWidgets/wxWidgetsMenubar.hpp"
#include "wxWidgets/wxWidgetsPanel.hpp"

namespace Nexus::UI
{
	wxWidgetsLayout::wxWidgetsLayout(wxFrame *frame) : m_Frame(frame)
	{
	}

	IPanel *wxWidgetsLayout::CreatePanel()
	{
		return AddChild<wxWidgetsPanel, IPanel>(m_Frame);
	}

	IMenubar *wxWidgetsLayout::CreateMainMenubar()
	{
		return AddChild<wxWidgetsMenubar, IMenubar>(m_Frame);
	}
}	 // namespace Nexus::UI