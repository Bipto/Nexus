#pragma once

namespace Nexus::Graphics
{
    enum class ImageLayout
    {
        Undefined,
        General,
        Colour,
        DepthRead,
        DepthReadWrite,
        ShaderResource,
        ResolveSource,
        ResolveDestination
    };
}