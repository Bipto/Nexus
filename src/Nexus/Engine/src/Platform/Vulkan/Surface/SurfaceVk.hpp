#pragma once

#include "RHI/ISurface.hpp"
#include "RHI/SwapchainDescription.hpp"

#include "../Vk.hpp"

namespace Nexus::Graphics
{
    class SurfaceVk : public ISurface
    {
      public:
        virtual ~SurfaceVk() = default;
        virtual std::expected<VkSurfaceKHR, VkResult> CreateVkSurface(
            const SwapchainDescription &swapchainDesc, VkInstance instance, const GladVulkanContext &context
        ) const = 0;
    };
} // namespace Nexus::Graphics