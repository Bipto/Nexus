#pragma once

#include <inttypes.h>

namespace Nexus::Graphics
{
	typedef uint64_t DeviceAddress;

	struct StridedDeviceAddressRegion
	{
		DeviceAddress Address = 0;
		size_t		  Stride  = 0;
		size_t		  Size	  = 0;
	};

}	 // namespace Nexus::Graphics