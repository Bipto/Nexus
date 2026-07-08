#pragma once

#include <inttypes.h>

namespace Nexus::Graphics
{
    typedef uint64_t DeviceAddress;

    struct DeviceAddressRegion
    {
        DeviceAddress Address = 0;
        uint64_t Size = 0;
    };

    struct StridedDeviceAddressRegion
    {
        DeviceAddress Address = 0;
        uint64_t Stride = 0;
        uint64_t Size = 0;
    };

} // namespace Nexus::Graphics