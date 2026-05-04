#include "wxWidgets/wxWidgetsPanel.hpp"

#include "wxWidgets/wxWidgetsMenubar.hpp"

namespace Nexus::UI
{
	wxWidgetsPanel::wxWidgetsPanel(wxFrame *frame) : m_Frame(frame), m_Panel(new wxPanel(frame))
	{
	}

	IButton *wxWidgetsPanel::CreateButton(std::string_view text)
	{
		return nullptr;
	}

	wxPanel *Nexus::UI::wxWidgetsPanel::GetPanel()
	{
		return m_Panel;
	}
}	 // namespace Nexus::UI