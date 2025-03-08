#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::UI
{
	class Canvas
	{
	  public:
	  private:
		Nexus::Point2D<uint32_t> m_Position = {0, 0};
		Nexus::Point2D<uint32_t> m_Size		= {0, 0};
	};
}	 // namespace Nexus::UI