#pragma once

#include <vector>

#include <memory>

#include <wx/aui/aui.h>
#include <wx/wx.h>

#include "UI/Control.hpp"
#include "UI/Layout.hpp"

#include "wxWidgets/wxWidgetsMenubar.hpp"
#include "wxWidgets/wxWidgetsStatusBar.hpp"

namespace Nexus::UI
{
	class wxWidgetsLayout final : public ILayout
	{
	  public:
		wxWidgetsLayout()		 = default;
		~wxWidgetsLayout() final = default;
		std::unique_ptr<IPanel> CreatePanel() final;
		IFrame				   *CreateFrame(const std::string &title) final;

		void LogFatal(const std::string &message) final;
		void LogError(const std::string &message) final;
		void LogWarning(const std::string &message) final;
		void LogMessage(const std::string &message) final;
		void LogInfo(const std::string &message) final;
		void ShowMessageBox(const std::string &title, const std::string &message) final;

	  private:
		std::unique_ptr<IFrame> m_Frame = nullptr;
	};
}	 // namespace Nexus::UI