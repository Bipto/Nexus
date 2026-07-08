#pragma once

#include "SurfaceVk.hpp"

namespace Nexus::Graphics
{
    class SurfaceX11_Vk final : public SurfaceVk
    {
      public:
        SurfaceX11_Vk(uintptr_t display, uint32_t screen, uint32_t window);
        virtual ~SurfaceX11_Vk() final = default;
        std::expected<VkSurfaceKHR, VkResult> CreateVkSurface(
            const SwapchainDescription &swapchainDesc, VkInstance instance, const GladVulkanContext &context
        ) const final;

      private:
        uintptr_t m_Display = {};
        uint32_t m_Screen = {};
        uint32_t m_Window = {};
    };
} // namespace Nexus::Graphics