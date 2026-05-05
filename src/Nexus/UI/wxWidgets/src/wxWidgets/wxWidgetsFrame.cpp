#include "wxWidgets/wxWidgetsFrame.hpp"
#include "wxWidgets/wxWidgetsMenubar.hpp"
#include "wxWidgets/wxWidgetsStatusBar.hpp"

namespace Nexus::UI
{
	wxWidgetsFrame::wxWidgetsFrame(const std::string &title)
		: m_Frame(new wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxFULL_REPAINT_ON_RESIZE))
	{
		m_Frame->Show(true);
	}

	IMenubar *wxWidgetsFrame::CreateMenubar()
	{
		m_Menubar = std::make_unique<wxWidgetsMenubar>(m_Frame);
		return m_Menubar.get();
	}

	IStatusBar *wxWidgetsFrame::CreateStatusbar()
	{
		m_StatusBar = std::make_unique<wxWidgetsStatusBar>(m_Frame);
		return m_StatusBar.get();
	}
}	 // namespace Nexus::UI