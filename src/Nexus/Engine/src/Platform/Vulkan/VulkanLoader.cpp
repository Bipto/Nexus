#include "VulkanLoader.hpp"

#include <vulkan/vulkan.h>

namespace Nexus::Vk
{
    void *GetNxInstanceProcAddr()
    {
        return (void *)&vkGetInstanceProcAddr;
    }

    void *GetNxDeviceProcAddr()
    {
        return (void *)&vkGetDeviceProcAddr;
    }
} // namespace Nexus::Vk