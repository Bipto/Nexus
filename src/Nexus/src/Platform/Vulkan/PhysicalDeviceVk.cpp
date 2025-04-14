#include "PhysicalDeviceVk.hpp"

namespace Nexus::Graphics
{
	PhysicalDeviceVk::PhysicalDeviceVk(VkPhysicalDevice device) : m_Device(device)
	{
		vkGetPhysicalDeviceProperties(m_Device, &m_VkPhysicalDeviceProperties);
		m_Name = m_VkPhysicalDeviceProperties.deviceName;
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
}	 // namespace Nexus::Graphics
