#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Control.hpp"
#include "Menubar.hpp"

namespace Nexus::UI
{
	class IPanel
	{
	  public:
		virtual ~IPanel()				  = default;
		virtual IMenubar *CreateMenubar() = 0;
	};
}	 // namespace Nexus::UI