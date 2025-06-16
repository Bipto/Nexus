#include "PhysicalDeviceVk.hpp"

namespace Nexus::Graphics
{
	PhysicalDeviceVk::PhysicalDeviceVk(VkPhysicalDevice device) : m_Device(device)
	{
		vkGetPhysicalDeviceProperties(m_Device, &m_VkPhysicalDeviceProperties);
		m_Name = m_VkPhysicalDeviceProperties.deviceName;

		uint32_t extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(m_Device, nullptr, &extensionCount, nullptr);
		m_DeviceExtensions.resize(extensionCount);
		vkEnumerateDeviceExtensionProperties(m_Device, nullptr, &extensionCount, m_DeviceExtensions.data());
	}

	PhysicalDeviceVk::~PhysicalDeviceVk()
	{
	}

	const std::string &PhysicalDeviceVk::GetDeviceName() const
	{
		return m_Name;
	}

	const PhysicalDeviceFeatures &PhysicalDeviceVk::GetPhysicalDeviceFeatures() const
	{
		return m_Features;
	}

	const PhysicalDeviceLimits &PhysicalDeviceVk::GetPhysicalDeviceLimits() const
	{
		return m_Limits;
	}

	VkPhysicalDevice PhysicalDeviceVk::GetVkPhysicalDevice() const
	{
		return m_Device;
	}

	const VkPhysicalDeviceProperties &PhysicalDeviceVk::GetVkPhysicalDeviceProperties() const
	{
		return m_VkPhysicalDeviceProperties;
	}

	bool PhysicalDeviceVk::IsExtensionSupported(const char *extension) const
	{
		for (const auto &ext : m_DeviceExtensions)
		{
			if (strcmp(ext.extensionName, extension) == 0)
			{
				return true;
			}
		}

		return false;
	}
}	 // namespace Nexus::Graphics
