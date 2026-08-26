#include "SurfaceWin32_Vk.hpp"

#include "Platform/Vulkan/PlatformVk.hpp"

namespace Nexus::Graphics
{
    SurfaceWin32_Vk::SurfaceWin32_Vk(uintptr_t hwnd, uintptr_t hdc, uintptr_t hinstance)
        : m_Hwnd(hwnd), m_Hdc(hdc), m_Hinstance(hinstance)
    {
    }

    std::expected<VkSurfaceKHR, VkResult> SurfaceWin32_Vk::CreateVkSurface(const SwapchainDescription &swapchainDesc,
                                                                           VkInstance instance,
                                                                           const GladVulkanContext &context) const
    {
        VkWin32SurfaceCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = reinterpret_cast<HWND>(m_Hwnd);
        createInfo.hinstance = reinterpret_cast<HINSTANCE>(m_Hinstance);

        VkSurfaceKHR surface;

        VkResult result = context.CreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface);

        if (result == VK_SUCCESS)
        {
            return surface;
        }
        else
        {
            return std::unexpected(result);
        }
    }

    const uintptr_t SurfaceWin32_Vk::GetHwnd() const
    {
        return m_Hwnd;
    }

    const uintptr_t SurfaceWin32_Vk::GetHdc() const
    {
        return m_Hdc;
    }

    const uintptr_t SurfaceWin32_Vk::GetHinstance() const
    {
        return m_Hinstance;
    }
} // namespace Nexus::Graphics