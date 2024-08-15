#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
/// @brief An enum representing the different types that can be contained within a vertex buffer
enum class ShaderDataType
{
    None = 0,
    Byte,
    Byte2,
    Byte4,
    NormByte,
    NormByte2,
    NormByte4,
    Float,
    Float2,
    Float3,
    Float4,
    Half,
    Half2,
    Half4,
    Int,
    Int2,
    Int3,
    Int4,
    SignedByte,
    SignedByte2,
    SignedByte4,
    SignedByteNormalized,
    SignedByte2Normalized,
    SignedByte4Normalized,
    Short,
    ShortNormalized,
    Short2,
    Short2Normalized,
    Short4,
    Short4Normalized,
    UInt,
    UInt2,
    UInt3,
    UInt4,
    UShort,
    UShort2,
    UShort4,
    UShortNormalized,
    UShort2Normalized,
    UShort4Normalized
};

/// @brief A method that returns the size of a shader data type in bytes
/// @param type An enum representing the data type to get the size of
/// @return An unsigned 32 bit integer representing the number of bytes taken by the data type
uint32_t GetShaderDataTypeSize(ShaderDataType type);
} // namespace Nexus::Graphics