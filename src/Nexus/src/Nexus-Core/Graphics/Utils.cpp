#include "Nexus-Core/Graphics/Utils.hpp"

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics::Utils
{
	Graphics::DeviceBufferOrHostAddressType ExtractDeviceBufferOrHostAddress(Graphics::DeviceBufferOrHostAddress address,
																			 Ref<Graphics::DeviceBuffer>		&deviceBuffer,
																			 Graphics::HostAddress				&hostAddress)
	{
		if (Ref<Graphics::DeviceBuffer> *buffer = std::get_if<Ref<DeviceBuffer>>(&address))
		{
			deviceBuffer = *buffer;
			return Graphics::DeviceBufferOrHostAddressType::DeviceBuffer;
		}
		else if (Graphics::HostAddress *hAddress = std::get_if<Graphics::HostAddress>(&address))
		{
			hostAddress = *hAddress;
			return Graphics::DeviceBufferOrHostAddressType::HostAddress;
		}
		else
		{
			throw std::runtime_error("Failed to extract device buffer or host address");
		}
	}
}	 // namespace Nexus::Graphics::Utils
