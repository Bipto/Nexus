#pragma once

#include <string>

#include "RHI/PhysicalDeviceFeatures.hpp"

namespace Nexus::Graphics
{
    class IPhysicalDevice
    {
      public:
        virtual ~IPhysicalDevice() = default;
        virtual const std::string &GetDeviceName() const = 0;
    };
} // namespace Nexus::Graphics