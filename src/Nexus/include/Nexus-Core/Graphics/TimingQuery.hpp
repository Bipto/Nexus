#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	class TimingQuery
	{
	  public:
		NX_API virtual ~TimingQuery()
		{
		}
		NX_API virtual void	 Resolve()				  = 0;
		NX_API virtual float GetElapsedMilliseconds() = 0;
	};
}	 // namespace Nexus::Graphics