#pragma once

#include <inttypes.h>

namespace Nexus
{
    enum ShaderDataType
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
        Mat3,
        Mat4
    };

    uint32_t GetShaderDataTypeSize(ShaderDataType type);
}