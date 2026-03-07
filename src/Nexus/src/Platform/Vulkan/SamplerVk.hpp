#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "GraphicsDeviceVk.hpp"
	#include "RHI/Sampler.hpp"
	#include "Vk.hpp"

namespace Nexus::Graphics
{
	class SamplerVk : public ISampler
	{
	  public:
		SamplerVk(GraphicsDeviceVk *device, const SamplerDescription &spec);
		virtual ~SamplerVk();
		virtual const SamplerDescription &GetSamplerDescription() override;
		VkSampler						  GetSampler();

	  private:
		SamplerDescription m_Description;
		VkSampler		   m_Sampler;
		GraphicsDeviceVk  *m_Device = nullptr;
	};
}	 // namespace Nexus::Graphics

#endif