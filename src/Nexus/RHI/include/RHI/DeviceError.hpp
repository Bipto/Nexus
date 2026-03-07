#pragma once

namespace Nexus::Graphics
{
	enum class DeviceError
	{
		None = 0,
		Unknown,
		OutOfHostMemory,
		OutOfDeviceMemory,
		DeviceLost,
		InvalidArg,
		FeatureNotPresent,
		FormatNotSupported
	};
}	 // namespace Nexus::Graphics