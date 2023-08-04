#pragma once

namespace Nexus::Graphics
{
    /// @brief An enum class that represents an available graphics API backend
    enum class GraphicsAPI
    {
        /// @brief No graphics API context will be created
        None,

        /// @brief Graphics will be created using OpenGL
        OpenGL,

        /// @brief Graphics will be created using DirectX11
        DirectX11,

        /// @brief Graphics will be created using Vulkan
        Vulkan
    };
}