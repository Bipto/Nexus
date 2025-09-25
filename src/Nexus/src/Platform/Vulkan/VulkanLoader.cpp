#include "VulkanLoader.hpp"

#include <vulkan/vulkan.h>

namespace Nexus::Vk
{
	void *GetNxInstanceProcAddr()
	{
		return &vkGetInstanceProcAddr;
	}

	void *GetNxDeviceProcAddr()
	{
		return &vkGetDeviceProcAddr;
	}
}	 // namespace Nexus::Vk