#pragma once

namespace Nexus
{

    /// @brief An enum value representing the format to use for a depth buffer
    enum class DepthFormat
    {
        /// @brief No depth buffer will be attached to the framebuffer
        None = 0,

        /// @brief Format where 24 bits will be allocated to the depth buffer and 8 bits will be allocated to the stencil buffer
        DEPTH24STENCIL8,

        /// @brief A default value for creating a depth buffer, defaults to DEPTH24STENCIL8
        Depth = DEPTH24STENCIL8,
    };
}