#include "wxWidgets/wxWidgetsLayout.hpp"

#include "wxWidgets/wxWidgetsFrame.hpp"
#include "wxWidgets/wxWidgetsMenubar.hpp"
#include "wxWidgets/wxWidgetsPanel.hpp"
#include "wxWidgets/wxWidgetsStatusBar.hpp"

namespace Nexus::UI
{
	std::unique_ptr<IPanel> wxWidgetsLayout::CreatePanel()
	{
		return nullptr;
	}

	IFrame *wxWidgetsLayout::CreateFrame(const std::string &title)
	{
		m_Frame = std::make_unique<wxWidgetsFrame>(title);
		return m_Frame.get();
	}

	void wxWidgetsLayout::LogFatal(const std::string &message)
	{
		wxLogFatalError(message);
	}
	void wxWidgetsLayout::LogError(const std::string &message)
	{
		wxLogError(message);
	}

	void wxWidgetsLayout::LogWarning(const std::string &message)
	{
		wxLogWarning(message);
	}

	void wxWidgetsLayout::LogMessage(const std::string &message)
	{
		wxLogMessage(message);
	}

	void wxWidgetsLayout::LogInfo(const std::string &message)
	{
		wxLogInfo(message);
	}

	void wxWidgetsLayout::ShowMessageBox(const std::string &title, const std::string &message)
	{
		wxMessageBox(title, message, wxOK | wxICON_INFORMATION);
	}

}	 // namespace Nexus::UI