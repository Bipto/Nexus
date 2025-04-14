#pragma once

#if defined(NX_PLATFORM_WGL)

	#include "Platform/Windows/WindowsInclude.hpp"
	#include "glad/glad_wgl.h"
	#include "Nexus-Core/Graphics/IPhysicalDevice.hpp"
	#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	class PhysicalDeviceWGL final : public IPhysicalDevice
	{
	  public:
		PhysicalDeviceWGL(const std::string &deviceName, const std::vector<std::string> &displayNames);
		virtual ~PhysicalDeviceWGL();
		const std::string					 &GetDeviceName() const final;
		virtual const PhysicalDeviceFeatures &GetPhysicalDeviceFeatures() const final;
		virtual const PhysicalDeviceLimits	 &GetPhysicalDeviceLimits() const final;

		HDC GetHDC() const;

	  private:
		std::string				 m_DeviceName	= {};
		std::vector<std::string> m_DisplayNames = {};
		PhysicalDeviceFeatures	 m_Features		= {};
		PhysicalDeviceLimits	 m_Limits		= {};

		HDC m_HDC = {};
	};
}	 // namespace Nexus::Graphics

#endif