#pragma once

#include "Nexus-Core/nxpch.hpp"
#include "Platform/IWindow.hpp"
#include "Vk.hpp"

namespace PlatformVk
{
    std::vector<const char *> GetRequiredExtensions();
    VkSurfaceKHR CreateSurface(
        const GladVulkanContext &context, VkInstance instance, Nexus::IWindow *window
    );

} // namespace PlatformVk