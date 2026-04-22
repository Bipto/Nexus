#include "wxWidgets/wxWidgetsPanel.hpp"

#include "wxWidgets/wxWidgetsMenubar.hpp"

namespace Nexus::UI
{
	wxWidgetsPanel::wxWidgetsPanel(wxFrame *frame) : m_Frame(frame), m_Panel(new wxPanel(frame))
	{
	}

	IMenubar *wxWidgetsPanel::CreateMenubar()
	{
		return new wxWidgetsMenubar(m_Frame);
	}

	wxPanel *Nexus::UI::wxWidgetsPanel::GetPanel()
	{
		return m_Panel;
	}
}	 // namespace Nexus::UI