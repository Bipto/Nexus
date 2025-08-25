#pragma once

#include "Nexus-Core/Utils/Utils.hpp"

namespace Nexus::Graphics
{
	enum QueueCapabilities
	{
		Invalid	 = 0,
		Graphics = BIT(0),
		Compute	 = BIT(2),
		Transfer = BIT(3),
		Present	 = BIT(4)
	};
}	 // namespace Nexus::Graphics