#if defined(NX_PLATFORM_VULKAN)

	#include "SamplerVk.hpp"

namespace Nexus::Graphics
{
	SamplerVk::SamplerVk(GraphicsDeviceVk *device, const SamplerSpecification &spec) : m_Device(device)
	{
		VkFilter			min, max;
		VkSamplerMipmapMode mipmapMode;
		Vk::GetVkFilterFromNexusFormat(spec.SampleFilter, min, max, mipmapMode);
		VkBorderColor color = Vk::GetVkBorderColor(spec.TextureBorderColor);

		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType				= VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.minFilter			= min;
		samplerInfo.magFilter			= max;
		samplerInfo.mipmapMode			= mipmapMode;
		samplerInfo.addressModeU		= Vk::GetVkSamplerAddressMode(spec.AddressModeU);
		samplerInfo.addressModeV		= Vk::GetVkSamplerAddressMode(spec.AddressModeV);
		samplerInfo.addressModeW		= Vk::GetVkSamplerAddressMode(spec.AddressModeW);

		if (spec.SampleFilter == Nexus::Graphics::SamplerFilter::Anisotropic)
		{
			samplerInfo.anisotropyEnable = VK_TRUE;
			samplerInfo.maxAnisotropy	 = spec.MaximumAnisotropy;
		}
		else
		{
			samplerInfo.anisotropyEnable = VK_FALSE;
		}

		samplerInfo.borderColor				= color;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable			= VK_TRUE;
		samplerInfo.compareOp				= Vk::GetCompareOp(spec.SamplerComparisonFunction);
		samplerInfo.mipLodBias				= mipmapMode;
		samplerInfo.mipLodBias				= spec.LODBias;
		samplerInfo.minLod					= spec.MinimumLOD;
		samplerInfo.maxLod					= spec.MaximumLOD;

		if (vkCreateSampler(device->GetVkDevice(), &samplerInfo, nullptr, &m_Sampler) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create sampler");
		}
	}

	SamplerVk::~SamplerVk()
	{
		vkDestroySampler(m_Device->GetVkDevice(), m_Sampler, nullptr);
	}

	const SamplerSpecification &SamplerVk::GetSamplerSpecification()
	{
		return m_Specification;
	}

	VkSampler SamplerVk::GetSampler()
	{
		return m_Sampler;
	}
}	 // namespace Nexus::Graphics

#endif