#include "SurfaceX11_Vk.hpp"

#include "Platform/Vulkan/PlatformVk.hpp"

namespace Nexus::Graphics
{
    SurfaceX11_Vk::SurfaceX11_Vk(uintptr_t display, uint32_t screen, uint32_t window)
        : m_Display(display), m_Screen(screen), m_Window(window)
    {
    }

    std::expected<VkSurfaceKHR, VkResult> SurfaceX11_Vk::CreateVkSurface(
        const SwapchainDescription &swapchainDesc, VkInstance instance,
        const GladVulkanContext &context
    ) const
    {
        Display *display = reinterpret_cast<Display *>(m_Display);
        int screen = static_cast<int>(m_Screen);
        Window window = static_cast<Window>(m_Window);

        VkXlibSurfaceCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
        createInfo.dpy = display;
        createInfo.window = window;

        VkSurfaceKHR surface = {};

        VkResult result =
            context.CreateXlibSurfaceKHR(instance, &createInfo, nullptr, &surface);

        if (result == VK_SUCCESS)
        {
            return surface;
        }
        else
        {
            return std::unexpected(result);
        }
    }
} // namespace Nexus::Graphics