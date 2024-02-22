#pragma once

namespace Nexus::Graphics
{
    struct GraphicsCapabilities
    {
        bool SupportsMultisampledFramebuffer = false;
        bool SupportsMultisampledSwapchain = false;
        bool SupportsLODBias = false;
    };
}