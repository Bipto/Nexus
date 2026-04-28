#pragma once

#include <memory>
#include <string>
#include <vector>

#include "UI/Control.hpp"
#include "UI/Menubar.hpp"

namespace Nexus::UI
{
	class IPanel
	{
	  public:
		virtual ~IPanel()				  = default;
		virtual IMenubar *CreateMenubar() = 0;
	};
}	 // namespace Nexus::UI