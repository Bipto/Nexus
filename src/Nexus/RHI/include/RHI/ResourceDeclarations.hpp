#pragma once

#include "Core/ResourcePool.hpp"

namespace Nexus::Graphics
{
    class IPipeline;
    DEFINE_RESOURCE(Pipeline, IPipeline);

    class IResourceSet;
    DEFINE_RESOURCE(ResourceSet, IResourceSet);
} // namespace Nexus::Graphics