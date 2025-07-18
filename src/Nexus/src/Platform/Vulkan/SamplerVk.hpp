#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "GraphicsDeviceVk.hpp"
	#include "Nexus-Core/Graphics/Sampler.hpp"
	#include "Vk.hpp"

namespace Nexus::Graphics
{
	class SamplerVk : public Sampler
	{
	  public:
		SamplerVk(GraphicsDeviceVk *device, const SamplerDescription &spec);
		virtual ~SamplerVk();
		virtual const SamplerDescription   &GetSamplerSpecification() override;
		VkSampler							GetSampler();

	  private:
		SamplerDescription	 m_Description;
		VkSampler			 m_Sampler;
		GraphicsDeviceVk	*m_Device = nullptr;
	};
}	 // namespace Nexus::Graphics

#endif