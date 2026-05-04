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

}	 // namespace Nexus::UI