#pragma once

#include <inttypes.h>

namespace Nexus::Graphics
{
    /// @brief An enum representing the different types that can be contained within a vertex buffer
    enum class ShaderDataType
    {
        None = 0,
        Float,
        Float2,
        Float3,
        Float4,
        Int,
        Int2,
        Int3,
        Int4,
    };

    /// @brief A method that returns the size of a shader data type in bytes
    /// @param type An enum representing the data type to get the size of
    /// @return An unsigned 32 bit integer representing the number of bytes taken by the data type
    uint32_t GetShaderDataTypeSize(ShaderDataType type);
}