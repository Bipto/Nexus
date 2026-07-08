#pragma once

#include "SurfaceVk.hpp"

namespace Nexus::Graphics
{
    class SurfaceWin32_Vk final : public SurfaceVk
    {
      public:
        SurfaceWin32_Vk(uintptr_t hwnd, uintptr_t hdc, uintptr_t hinstance);
        virtual ~SurfaceWin32_Vk() final = default;
        std::expected<VkSurfaceKHR, VkResult> CreateVkSurface(
            const SwapchainDescription &swapchainDesc, VkInstance instance,
            const GladVulkanContext &context
        ) const final;

        const uintptr_t GetHwnd() const;
        const uintptr_t GetHdc() const;
        const uintptr_t GetHinstance() const;

      private:
        uintptr_t m_Hwnd = 0;
        uintptr_t m_Hdc = 0;
        uintptr_t m_Hinstance = 0;
    };
} // namespace Nexus::Graphics