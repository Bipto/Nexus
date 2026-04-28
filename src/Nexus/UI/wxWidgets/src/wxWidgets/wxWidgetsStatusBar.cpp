#include "wxWidgets/wxWidgetsStatusBar.hpp"

namespace Nexus::UI
{
	wxWidgetsStatusBar::wxWidgetsStatusBar(wxFrame *frame) : m_Frame(frame), m_StatusBar(m_Frame->CreateStatusBar())
	{
	}

	void wxWidgetsStatusBar::SetStatusText(std::string_view text)
	{
		std::string textStr {text};
		m_StatusBar->SetStatusText(textStr);
	}

	void wxWidgetsStatusBar::SetHelpText(std::string_view text)
	{
		std::string textStr {text};
		m_StatusBar->SetHelpText(textStr);
	}
}	 // namespace Nexus::UI