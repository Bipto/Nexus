#pragma once

#include "SamplerState.hpp"

namespace Nexus::Graphics
{
	class ISampler
	{
	  public:
		virtual ~ISampler()
		{
		}
		virtual const SamplerDescription &GetSamplerDescription() = 0;
	};
}	 // namespace Nexus::Graphics