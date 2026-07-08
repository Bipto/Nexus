#pragma once

#include <string>

#include "Core/ResourcePool.hpp"

#include "RHI/RHI-Core.hpp"

namespace Nexus::Graphics
{
    enum class FenceWaitResult
    {
        TimedOut,
        Signalled,
        Failed
    };

    struct FenceDescription
    {
        bool Signalled = false;
        std::string DebugName = "Fence";
    };

    class NX_RHI_API IFence
    {
      public:
        virtual ~IFence()
        {
        }
        virtual bool IsSignalled() const = 0;
        virtual const FenceDescription &GetDescription() const = 0;
    };

    DEFINE_RESOURCE(Fence, IFence);
} // namespace Nexus::Graphics