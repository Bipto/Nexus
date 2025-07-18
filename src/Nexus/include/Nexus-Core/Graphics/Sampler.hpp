#pragma once

#include "SamplerState.hpp"

namespace Nexus::Graphics
{
	class Sampler
	{
	  public:
		virtual ~Sampler()
		{
		}
		virtual const SamplerDescription &GetSamplerSpecification() = 0;
	};
}	 // namespace Nexus::Graphics