#pragma once

#include <string_view>

#include "UI/Menubar.hpp"
#include "UI/Panel.hpp"
#include "UI/StatusBar.hpp"

namespace Nexus::UI
{
	class ILayout
	{
	  public:
		virtual ~ILayout()						= default;
		virtual IPanel	   *CreatePanel()		= 0;
		virtual IMenubar   *CreateMainMenubar() = 0;
		virtual IStatusBar *CreateStatusBar()	= 0;
	};
}	 // namespace Nexus::UI