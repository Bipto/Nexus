#pragma once

#include <memory>

#include "SamplerState.hpp"

#include "Core/ResourcePool.hpp"

namespace Nexus::Graphics
{
	class ISampler
	{
	  public:
		virtual ~ISampler()
		{
		}
		virtual const SamplerDescription &GetSamplerDescription() const = 0;
	};

	DEFINE_RESOURCE(Sampler, ISampler);
}	 // namespace Nexus::Graphics