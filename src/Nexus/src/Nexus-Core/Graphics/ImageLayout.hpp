#pragma once

namespace Nexus::Graphics
{
    enum class ImageLayout
    {
        Undefined = 0,
        General,
        Colour,
        DepthRead,
        DepthReadWrite,
        ShaderResource,
        ResolveSource,
        ResolveDestination
    };
}