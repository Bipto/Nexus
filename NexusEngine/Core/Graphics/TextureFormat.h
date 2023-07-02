#pragma once

namespace Nexus
{
    /// @brief An enum representing how the texture will be stored in memory
    enum class TextureFormat
    {
        /// @brief A default value that represents no data being stored
        None = 0,

        /// @brief A value representing that 8 bits will be allocated for the red, green, blue and alpha channels
        RGBA8,

        /// @brief A default value for creating colour textures
        Color = RGBA8,
    };
}