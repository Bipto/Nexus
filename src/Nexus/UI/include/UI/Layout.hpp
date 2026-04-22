#pragma once

#include "UI/Panel.hpp"

namespace Nexus::UI
{
	class ILayout
	{
	  public:
		virtual ~ILayout()			  = default;
		virtual IPanel *CreatePanel() = 0;
	};
}	 // namespace Nexus::UI