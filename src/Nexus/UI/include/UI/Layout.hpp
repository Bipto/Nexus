#pragma once

#include <string>
#include <string_view>

#include "UI/Frame.hpp"
#include "UI/Panel.hpp"
#include "UI/StatusBar.hpp"

namespace Nexus::UI
{
	class ILayout
	{
	  public:
		virtual ~ILayout()														  = default;
		virtual std::unique_ptr<IPanel>		CreatePanel()						  = 0;
		virtual std::unique_ptr<IStatusBar> CreateStatusBar()					  = 0;
		virtual std::unique_ptr<IFrame>		CreateFrame(const std::string &title) = 0;
	};
}	 // namespace Nexus::UI