#include "wxWidgets/wxWidgetsFrame.hpp"
#include "wxWidgets/wxWidgetsMenubar.hpp"

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
}	 // namespace Nexus::UI