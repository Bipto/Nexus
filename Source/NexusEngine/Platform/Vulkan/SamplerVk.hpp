#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "Vk.hpp"

#include "GraphicsDeviceVk.hpp"

#include "Nexus/Graphics/Sampler.hpp"

namespace Nexus::Graphics
{
    class SamplerVk : public Sampler
    {
    public:
        SamplerVk(GraphicsDeviceVk *device, const SamplerSpecification &spec);
        virtual ~SamplerVk();
        virtual const SamplerSpecification &GetSamplerSpecification() override;
        VkSampler GetSampler();

    private:
        SamplerSpecification m_Specification;
        VkSampler m_Sampler;
    };
}

#endif