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

	struct SamplerTag
	{
	};
	using SamplerID		= Nexus::HandleT<SamplerTag>;
	using SamplerHandle = SharedHandle<ISampler, SamplerID>;
	using SamplerPool	= ResourcePool<ISampler, SamplerID>;
}	 // namespace Nexus::Graphics