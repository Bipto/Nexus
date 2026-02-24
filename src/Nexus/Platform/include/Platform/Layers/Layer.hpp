#pragma once

#include "Platform/Events/Event.hpp"

namespace Nexus
{
	class ILayer
	{
	  public:
		virtual bool OnEvent(const Event &event) = 0;
	};
}	 // namespace Nexus
