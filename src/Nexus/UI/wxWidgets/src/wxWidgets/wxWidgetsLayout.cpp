#include "wxWidgets/wxWidgetsLayout.hpp"

#include "wxWidgets/wxWidgetsPanel.hpp"

namespace Nexus::UI
{
	wxWidgetsLayout::wxWidgetsLayout(wxFrame *frame) : m_Frame(frame)
	{
	}

	IPanel *wxWidgetsLayout::CreatePanel()
	{
		return new wxWidgetsPanel(m_Frame);
	}
}	 // namespace Nexus::UI