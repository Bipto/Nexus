#pragma once

#include "Nexus-Core/Graphics/AccelerationStructure.hpp"

namespace Nexus::Graphics::Utils
{
	DeviceBufferOrHostAddressType ExtractDeviceBufferOrHostAddress(Graphics::DeviceBufferOrHostAddress address,
																   Graphics::DeviceBufferAddress	  &deviceBuffer,
																   Graphics::HostAddress			  &hostAddress);
}