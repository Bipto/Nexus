#pragma once

#include "Nexus-Core/Graphics/AccelerationStructure.hpp"

namespace Nexus::Graphics::Utils
{
	DeviceBufferOrHostAddressType ExtractDeviceBufferOrHostAddress(Graphics::DeviceBufferOrHostAddress address,
																   Ref<Graphics::DeviceBuffer>		  &deviceBuffer,
																   Graphics::HostAddress			  &hostAddress);
}