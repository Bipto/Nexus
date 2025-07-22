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
		const std::string &GetDeviceName() const final;

		VkPhysicalDevice				  GetVkPhysicalDevice() const;
		const VkPhysicalDeviceProperties &GetVkPhysicalDeviceProperties() const;

		bool IsExtensionSupported(const char *extension) const;
		bool IsVersionGreaterThan(uint32_t version) const;

	  private:
		std::string			   m_Name	  = {};
		VkPhysicalDevice	   m_Device	  = nullptr;

		VkPhysicalDeviceProperties m_VkPhysicalDeviceProperties = {};
		std::vector<VkExtensionProperties> m_DeviceExtensions			= {};
	};
}	 // namespace Nexus::Graphics

#endif