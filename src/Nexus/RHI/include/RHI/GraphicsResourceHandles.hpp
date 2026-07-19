#pragma once

#include "Core/ResourcePool.hpp"

namespace Nexus::Graphics
{
    class ICommandQueue;
    DEFINE_RESOURCE(CommandQueue, ICommandQueue);

    class ICommandList;
    DEFINE_RESOURCE(CommandList, ICommandList);
} // namespace Nexus::Graphics