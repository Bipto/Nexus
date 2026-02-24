#pragma once

#include <cstdint>

#include "Platform/Timings/DateTime.hpp"

namespace Nexus
{
	struct Event
	{
		virtual ~Event() = default;
	};
}	 // namespace Nexus