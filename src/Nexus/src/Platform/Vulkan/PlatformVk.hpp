#pragma once

#include "Nexus-Core/Window.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "Vk.hpp"

namespace PlatformVk
{
	std::vector<const char *> GetRequiredExtensions();
	VkSurfaceKHR			  CreateSurface(VkInstance instance, Nexus::IWindow *window);

}	 // namespace PlatformVk