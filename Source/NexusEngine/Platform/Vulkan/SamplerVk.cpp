#include "SamplerVk.hpp"

namespace Nexus::Graphics
{
    SamplerVk::SamplerVk(GraphicsDeviceVk *device, const SamplerSpecification &spec)
    {
        VkFilter min, max;
        VkSamplerMipmapMode mipmapMode;
        GetVkFilterFromNexusFormat(spec.SampleFilter, min, max, mipmapMode);
        VkBorderColor color = GetVkBorderColor(spec.BorderColor);

        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.minFilter = min;
        samplerInfo.magFilter = max;
        samplerInfo.mipmapMode = mipmapMode;
        samplerInfo.addressModeU = GetVkSamplerAddressMode(spec.AddressModeU);
        samplerInfo.addressModeV = GetVkSamplerAddressMode(spec.AddressModeV);
        samplerInfo.addressModeW = GetVkSamplerAddressMode(spec.AddressModeW);

        if (spec.SampleFilter == Nexus::Graphics::SamplerFilter::Anisotropic)
        {
            samplerInfo.anisotropyEnable = VK_TRUE;
            samplerInfo.maxAnisotropy = spec.MaximumAnisotropy;
        }
        else
        {
            samplerInfo.anisotropyEnable = VK_FALSE;
        }

        samplerInfo.borderColor = color;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_TRUE;
        samplerInfo.compareOp = GetCompareOp(spec.ComparisonFunction);
        samplerInfo.mipLodBias = mipmapMode;
        samplerInfo.mipLodBias = spec.LODBias;
        samplerInfo.minLod = spec.MinimumLOD;
        samplerInfo.maxLod = spec.MaximumLOD;

        if (vkCreateSampler(device->GetVkDevice(), &samplerInfo, nullptr, &m_Sampler) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create sampler");
        }
    }

    SamplerVk::~SamplerVk()
    {
    }

    const SamplerSpecification &SamplerVk::GetSamplerSpecification()
    {
        return m_Specification;
    }

    VkSampler SamplerVk::GetSampler()
    {
        return m_Sampler;
    }
}