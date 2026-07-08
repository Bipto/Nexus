#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "GraphicsDeviceVk.hpp"
#include "RHI/Sampler.hpp"
#include "Vk.hpp"

namespace Nexus::Graphics
{
    class SamplerVk final : public ISampler
    {
      public:
        SamplerVk(GraphicsDeviceVk *device, const SamplerDescription &spec);
        virtual ~SamplerVk();
        const SamplerDescription &GetSamplerDescription() const final;
        const VkSampler GetSampler() const;

      private:
        SamplerDescription m_Description;
        VkSampler m_Sampler;
        GraphicsDeviceVk *m_Device = nullptr;
    };
} // namespace Nexus::Graphics

#endif