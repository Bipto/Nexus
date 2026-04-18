#pragma once

#include "Core/ResourcePool.hpp"

#include "RHI/RHI-Core.hpp"

namespace Nexus::Graphics
{
	class NX_RHI_API ITimingQuery
	{
	  public:
		virtual ~ITimingQuery()
		{
		}
		virtual void  Resolve()				   = 0;
		virtual float GetElapsedMilliseconds() = 0;
	};

	DEFINE_RESOURCE(TimingQuery, ITimingQuery);
}	 // namespace Nexus::Graphics