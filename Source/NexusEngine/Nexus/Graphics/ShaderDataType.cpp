#include "ShaderDataType.hpp"

namespace Nexus::Graphics
{
    uint32_t GetShaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Byte:
            return 1;
            break;
        case ShaderDataType::Byte2:
            return 1 * 2;
            break;
        case ShaderDataType::Byte4:
            return 1 * 4;
            break;
        case ShaderDataType::NormByte:
            return 1;
            break;
        case ShaderDataType::NormByte2:
            return 1 * 2;
            break;
        case ShaderDataType::NormByte4:
            return 1 * 4;
            break;
        case ShaderDataType::Float:
            return 4;
            break;
        case ShaderDataType::Float2:
            return 4 * 2;
            break;
        case ShaderDataType::Float3:
            return 4 * 3;
            break;
        case ShaderDataType::Float4:
            return 4 * 4;
            break;
        case ShaderDataType::Half:
            return 2;
            break;
        case ShaderDataType::Half2:
            return 2 * 2;
            break;
        case ShaderDataType::Half4:
            return 2 * 4;
            break;
        case ShaderDataType::Int:
            return 4;
            break;
        case ShaderDataType::Int2:
            return 4 * 2;
            break;
        case ShaderDataType::Int3:
            return 4 * 3;
            break;
        case ShaderDataType::Int4:
            return 4 * 4;
            break;
        case ShaderDataType::SignedByte:
            return 1;
            break;
        case ShaderDataType::SignedByte2:
            return 1 * 2;
            break;
        case ShaderDataType::SignedByte4:
            return 1 * 4;
            break;
        case ShaderDataType::SignedByteNormalized:
            return 1;
            break;
        case ShaderDataType::SignedByte2Normalized:
            return 1 * 2;
            break;
        case ShaderDataType::SignedByte4Normalized:
            return 1 * 4;
            break;
        case ShaderDataType::Short:
            return 2;
            break;
        case ShaderDataType::ShortNormalized:
            return 2;
            break;
        case ShaderDataType::Short2:
            return 2 * 2;
            break;
        case ShaderDataType::Short2Normalized:
            return 2 * 2;
            break;
        case ShaderDataType::Short4:
            return 2 * 4;
            break;
        case ShaderDataType::Short4Normalized:
            return 2 * 4;
            break;
        case ShaderDataType::UInt:
            return 4;
            break;
        case ShaderDataType::UInt2:
            return 4 * 2;
            break;
        case ShaderDataType::UInt3:
            return 4 * 3;
            break;
        case ShaderDataType::UInt4:
            return 4 * 4;
            break;
        case ShaderDataType::UShort:
            return 2;
            break;
        case ShaderDataType::UShort2:
            return 2 * 2;
            break;
        case ShaderDataType::UShort4:
            return 2 * 4;
            break;
        case ShaderDataType::UShortNormalized:
            return 2;
            break;
        case ShaderDataType::UShort2Normalized:
            return 2 * 2;
            break;
        case ShaderDataType::UShort4Normalized:
            return 2 * 4;
            break;
        }
    }
}