#pragma once

#include "UI/Menubar.hpp"
#include "UI/Panel.hpp"

namespace Nexus::UI
{
	class ILayout : public IControl
	{
	  public:
		virtual ~ILayout()					  = default;
		virtual IPanel	 *CreatePanel()		  = 0;
		virtual IMenubar *CreateMainMenubar() = 0;
	};
}	 // namespace Nexus::UI