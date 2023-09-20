#pragma once

namespace Nexus::Graphics
{
    /// @brief A struct representing a viewport to use when rendering
    struct Viewport
    {
        /// @brief The X position of the viewport
        int X = 0;

        /// @brief The Y position of the viewport
        int Y = 0;

        /// @brief The width of the viewport
        int Width = 0;

        /// @brief The height of the viewport
        int Height = 0;

        /// @brief The minimum depth of the viewport
        float MinDepth = 0.0f;

        /// @brief The maximum depth of the viewport
        float MaxDepth = 1.0f;
    };
}