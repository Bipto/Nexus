#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "Nexus-Core/Graphics/IPhysicalDevice.hpp"
	#include "Vk.hpp"

namespace Nexus::Graphics
{
	class PhysicalDeviceVk final : public IPhysicalDevice
	{
	  public:
		PhysicalDeviceVk(VkPhysicalDevice device);
		virtual ~PhysicalDeviceVk();
		const std::string					 &GetDeviceName() const final;
		virtual const PhysicalDeviceFeatures &GetPhysicalDeviceFeatures() const final;
		virtual const PhysicalDeviceLimits	 &GetPhysicalDeviceLimits() const final;

		VkPhysicalDevice				  GetVkPhysicalDevice() const;
		const VkPhysicalDeviceProperties &GetVkPhysicalDeviceProperties() const;

		bool IsExtensionSupported(const char *extension) const;

	  private:
		std::string			   m_Name	  = {};
		VkPhysicalDevice	   m_Device	  = nullptr;
		PhysicalDeviceFeatures m_Features = {};
		PhysicalDeviceLimits   m_Limits	  = {};

		VkPhysicalDeviceProperties m_VkPhysicalDeviceProperties = {};
		std::vector<VkExtensionProperties> m_DeviceExtensions			= {};
	};
}	 // namespace Nexus::Graphics

#endif