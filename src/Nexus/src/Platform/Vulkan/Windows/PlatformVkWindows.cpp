
#include "Platform/Vulkan/PlatformVk.hpp"

namespace PlatformVk
{
	std::vector<const char *> GetRequiredExtensions()
	{
		return {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME};
	}

	VkSurfaceKHR CreateSurface(const GladVulkanContext &context, VkInstance instance, Nexus::IWindow *window)
	{
		const auto &nativeWindowInfo = window->GetNativeWindowInfo();

		VkWin32SurfaceCreateInfoKHR createInfo = {};
		createInfo.sType					   = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd						   = nativeWindowInfo.hwnd;
		createInfo.hinstance				   = nativeWindowInfo.instance;

		VkSurfaceKHR surface;

		if (context.CreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create Vulkan surface");
		}

		return surface;
	}

}	 // namespace PlatformVk