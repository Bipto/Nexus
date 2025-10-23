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
		virtual const SamplerDescription &GetSamplerSpecification() = 0;
	};
}	 // namespace Nexus::Graphics