#include "Platform/Vulkan/PlatformVk.hpp"

#include <android/native_window.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_android.h>


namespace PlatformVk
{
	std::vector<const char *> GetRequiredExtensions()
	{
		return {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_ANDROID_SURFACE_EXTENSION_NAME};
	}

	VkSurfaceKHR CreateSurface(VkInstance instance, Nexus::IWindow *window)
	{
		const auto &nativeWindowInfo = window->GetNativeWindowInfo();

		VkAndroidSurfaceCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
        createInfo.pNext = nullptr;
        createInfo.window = nativeWindowInfo.nativeWindow;

		VkSurfaceKHR surface;

		if (vkCreateAndroidSurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create Vulkan surface");
		}

		return surface;
	}

}	 // namespace PlatformVk