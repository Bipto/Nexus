#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	class ITimingQuery
	{
	  public:
		virtual ~ITimingQuery()
		{
		}
		virtual void  Resolve()				   = 0;
		virtual float GetElapsedMilliseconds() = 0;
	};
}	 // namespace Nexus::Graphics