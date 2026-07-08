#include "RHI/VertexBufferLayout.hpp"

namespace Nexus::Graphics
{
    VertexBufferElement::VertexBufferElement(
        ShaderDataType type, const std::string &name
    )
        : Name(name), Type(type), Size(GetShaderDataTypeSize(type)), Offset(0)
    {
    }

    uint32_t VertexBufferElement::GetComponentCount() const
    {
        switch (Type)
        {
        case ShaderDataType::R8_UInt:
            return 1;
            break;
        case ShaderDataType::R8G8_UInt:
            return 1 * 2;
            break;
        case ShaderDataType::R8G8B8A8_UInt:
            return 1 * 4;
            break;
        case ShaderDataType::R8_UNorm:
            return 1;
            break;
        case ShaderDataType::R8G8_UNorm:
            return 1 * 2;
            break;
        case ShaderDataType::R8G8B8A8_UNorm:
            return 1 * 4;
            break;
        case ShaderDataType::R32_SFloat:
            return 1;
            break;
        case ShaderDataType::R32G32_SFloat:
            return 1 * 2;
            break;
        case ShaderDataType::R32G32B32_SFloat:
            return 1 * 3;
            break;
        case ShaderDataType::R32G32B32A32_SFloat:
            return 1 * 4;
            break;
        case ShaderDataType::R16_SFloat:
            return 1;
            break;
        case ShaderDataType::R16G16_SFloat:
            return 1 * 2;
            break;
        case ShaderDataType::R16G16B16A16_SFloat:
            return 1 * 4;
            break;
        case ShaderDataType::R32_SInt:
            return 1;
            break;
        case ShaderDataType::R32G32_SInt:
            return 1 * 2;
            break;
        case ShaderDataType::R32G32B32_SInt:
            return 1 * 3;
            break;
        case ShaderDataType::R32G32B32A32_SInt:
            return 1 * 4;
            break;
        case ShaderDataType::R8_SInt:
            return 1;
            break;
        case ShaderDataType::R8G8_SInt:
            return 1 * 2;
            break;
        case ShaderDataType::R8G8B8A8_SInt:
            return 1 * 4;
            break;
        case ShaderDataType::R8_SNorm:
            return 1;
            break;
        case ShaderDataType::R8G8_SNorm:
            return 1 * 2;
            break;
        case ShaderDataType::R8G8B8A8_SNorm:
            return 1 * 4;
            break;
        case ShaderDataType::R16_SInt:
            return 1;
            break;
        case ShaderDataType::R16_SNorm:
            return 1;
            break;
        case ShaderDataType::R16G16_SInt:
            return 1 * 2;
            break;
        case ShaderDataType::R16G16_SNorm:
            return 1 * 2;
            break;
        case ShaderDataType::R16G16B16A16_SInt:
            return 1 * 4;
            break;
        case ShaderDataType::R16G16B16A16_SNorm:
            return 1 * 4;
            break;
        case ShaderDataType::R32_UInt:
            return 1;
            break;
        case ShaderDataType::R32G32_UInt:
            return 1 * 2;
            break;
        case ShaderDataType::R32G32B32_UInt:
            return 1 * 3;
            break;
        case ShaderDataType::R32G32B32A32_UInt:
            return 1 * 4;
            break;
        case ShaderDataType::R16_UInt:
            return 1;
            break;
        case ShaderDataType::R16G16_UInt:
            return 1 * 2;
            break;
        case ShaderDataType::R16G16B16A16_UInt:
            return 1 * 4;
            break;
        case ShaderDataType::R16_UNorm:
            return 1;
            break;
        case ShaderDataType::R16G16_UNorm:
            return 1 * 2;
            break;
        case ShaderDataType::R16G16B16A16_UNorm:
            return 1 * 4;
            break;
        default:
            return 0;
            break;
        }
    }

    bool VertexBufferLayout::IsVertexBuffer() const
    {
        return m_StepRate == StepRate::Vertex;
    }

    bool VertexBufferLayout::IsInstanceBuffer() const
    {
        return m_StepRate == StepRate::Instance;
    }

    void VertexBufferLayout::CalculateOffsets()
    {
        size_t offset = 0;
        for (auto &element : m_Elements)
        {
            element.Offset = offset;
            offset += element.Size;
        }
    }

    int GetIndexInArray(int index, int arraySize)
    {
        auto validIndex = index + 1;
        validIndex = validIndex % arraySize;
        return validIndex;
    }
} // namespace Nexus::Graphics