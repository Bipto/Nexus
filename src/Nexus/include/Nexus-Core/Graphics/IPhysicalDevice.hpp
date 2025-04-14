#pragma once

#include "Nexus-Core/Graphics/PhysicalDeviceFeatures.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	class IPhysicalDevice
	{
	  public:
		virtual ~IPhysicalDevice()												= default;
		virtual const std::string			 &GetDeviceName() const				= 0;
		virtual const PhysicalDeviceFeatures &GetPhysicalDeviceFeatures() const = 0;
		virtual const PhysicalDeviceLimits	 &GetPhysicalDeviceLimits() const	= 0;
	};
}	 // namespace Nexus::Graphics