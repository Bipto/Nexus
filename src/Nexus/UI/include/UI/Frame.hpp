#pragma once

#include <memory>

#include "UI/Menubar.hpp"

namespace Nexus::UI
{
	class IFrame
	{
	  public:
		virtual ~IFrame()				  = default;
		virtual IMenubar *CreateMenubar() = 0;
	};
}	 // namespace Nexus::UI