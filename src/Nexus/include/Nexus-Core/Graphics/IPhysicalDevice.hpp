#pragma once

#include "Nexus-Core/Graphics/PhysicalDeviceFeatures.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	class IPhysicalDevice
	{
	  public:
		virtual ~IPhysicalDevice()												= default;
		virtual const std::string &GetDeviceName() const						= 0;
	};
}	 // namespace Nexus::Graphics