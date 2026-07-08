#pragma once

#include "Core/ResourcePool.hpp"

#include "RHI/RHI-Core.hpp"

namespace Nexus::Graphics
{
    class NX_RHI_API ISurface
    {
      public:
        virtual ~ISurface() = default;
    };

    DEFINE_RESOURCE(Surface, ISurface);
} // namespace Nexus::Graphics