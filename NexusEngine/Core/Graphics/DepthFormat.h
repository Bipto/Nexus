#pragma once

namespace Nexus
{
    enum class DepthFormat
    {
        None = 0,
        DEPTH24STENCIL8,

        //Default
        Depth = DEPTH24STENCIL8,
    };
}