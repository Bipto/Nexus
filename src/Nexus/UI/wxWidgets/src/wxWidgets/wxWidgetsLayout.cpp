#include "wxWidgets/wxWidgetsLayout.hpp"

#include "wxWidgets/wxWidgetsFrame.hpp"
#include "wxWidgets/wxWidgetsMenubar.hpp"
#include "wxWidgets/wxWidgetsPanel.hpp"
#include "wxWidgets/wxWidgetsStatusBar.hpp"

namespace Nexus::UI
{
	std::unique_ptr<IPanel> wxWidgetsLayout::CreatePanel()
	{
		// return std::make_unique<wxWidgetsPanel>(m_Frame);

		return nullptr;
	}

	std::unique_ptr<IStatusBar> wxWidgetsLayout::CreateStatusBar()
	{
		// return std::make_unique<wxWidgetsStatusBar>(m_Frame);

		return nullptr;
	}

	std::unique_ptr<IFrame> wxWidgetsLayout::CreateFrame(const std::string &title)
	{
		return std::make_unique<wxWidgetsFrame>(title);
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