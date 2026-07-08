#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "Platform/OpenGL/Context/IOffscreenContext.hpp"
#include "RHI/IPhysicalDevice.hpp"

namespace Nexus::Graphics
{
    class PhysicalDeviceOpenGL : public IPhysicalDevice
    {
      public:
        virtual GL::IOffscreenContext *GetOffscreenContext() = 0;
    };
} // namespace Nexus::Graphics

#endif