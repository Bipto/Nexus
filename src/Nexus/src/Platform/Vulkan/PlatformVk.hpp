#pragma once

#include "Nexus-Core/IWindow.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "Vk.hpp"

namespace PlatformVk
{
	std::vector<const char *> GetRequiredExtensions();
	VkSurfaceKHR			  CreateSurface(const GladVulkanContext &context, VkInstance instance, Nexus::IWindow *window);

}	 // namespace PlatformVk