#include "Platform/Vulkan/PlatformVk.hpp"

namespace PlatformVk
{
	std::vector<const char *> GetRequiredExtensions()
	{
		return {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XLIB_SURFACE_EXTENSION_NAME};
	}

	VkSurfaceKHR CreateSurface(VkInstance instance, Nexus::IWindow *window)
	{
		const auto &nativeWindowInfo = window->GetNativeWindowInfo();

		VkXlibSurfaceCreateInfoKHR createInfo = {};
		createInfo.sType					  = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
		createInfo.dpy						  = nativeWindowInfo.display;
		createInfo.window					  = nativeWindowInfo.window;

		VkSurfaceKHR surface;

		if (vkCreateXlibSurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create Vulkan surface");
		}

		return surface;
	}

}	 // namespace PlatformVk