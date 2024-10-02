#pragma once

#include "Nexus-Core/Point.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
	struct Monitor
	{
		Point2D<int32_t> Position	  = {0, 0};
		Point2D<int32_t> Size		  = {0, 0};
		Point2D<int32_t> WorkPosition = {0, 0};
		Point2D<int32_t> WorkSize {0, 0};
		float			 DPI = 0.0f;
		float			 RefreshRate = 0.0f;
		std::string		 Name;
	};
}	 // namespace Nexus