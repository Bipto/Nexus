#include "Vk.hpp"

#if defined(NX_PLATFORM_VULKAN)

VkFormat GetVkTextureFormatFromNexusFormat(Nexus::Graphics::TextureFormat format)
{
    switch (format)
    {
    case Nexus::Graphics::TextureFormat::RGBA8:
        return VK_FORMAT_R8G8B8A8_UNORM;
    case Nexus::Graphics::TextureFormat::R8:
        return VK_FORMAT_R8_UNORM;

    default:
        throw std::runtime_error("Failed to find a valid format");
        break;
    }
}

VkFormat GetVkDepthFormatFromNexusFormat(Nexus::Graphics::DepthFormat format)
{
    switch (format)
    {
    case Nexus::Graphics::DepthFormat::DEPTH24STENCIL8:
        return VK_FORMAT_D24_UNORM_S8_UINT;

    default:
        throw std::runtime_error("Failed to find a valid format");
        break;
    }
}

VkFormat GetShaderDataType(Nexus::Graphics::ShaderDataType type)
{
    switch (type)
    {
    case Nexus::Graphics::ShaderDataType::Byte:
        return VK_FORMAT_R8_UINT;
        break;
    case Nexus::Graphics::ShaderDataType::Byte2:
        return VK_FORMAT_R8G8_UINT;
        break;
    case Nexus::Graphics::ShaderDataType::Byte4:
        return VK_FORMAT_R8G8B8A8_UINT;
        break;

    case Nexus::Graphics::ShaderDataType::NormByte:
        return VK_FORMAT_R8_UNORM;
        break;
    case Nexus::Graphics::ShaderDataType::NormByte2:
        return VK_FORMAT_R8G8_UNORM;
        break;
    case Nexus::Graphics::ShaderDataType::NormByte4:
        return VK_FORMAT_R8G8B8A8_UNORM;
        break;

    case Nexus::Graphics::ShaderDataType::Float:
        return VK_FORMAT_R32_SFLOAT;
        break;
    case Nexus::Graphics::ShaderDataType::Float2:
        return VK_FORMAT_R32G32_SFLOAT;
        break;
    case Nexus::Graphics::ShaderDataType::Float3:
        return VK_FORMAT_R32G32B32_SFLOAT;
        break;
    case Nexus::Graphics::ShaderDataType::Float4:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
        break;

    case Nexus::Graphics::ShaderDataType::Half:
        return VK_FORMAT_R16_SFLOAT;
        break;
    case Nexus::Graphics::ShaderDataType::Half2:
        return VK_FORMAT_R16G16_SFLOAT;
        break;
    case Nexus::Graphics::ShaderDataType::Half4:
        return VK_FORMAT_R16G16B16A16_SFLOAT;
        break;

    case Nexus::Graphics::ShaderDataType::Int:
        return VK_FORMAT_R32_SINT;
        break;
    case Nexus::Graphics::ShaderDataType::Int2:
        return VK_FORMAT_R32G32_SINT;
        break;
    case Nexus::Graphics::ShaderDataType::Int3:
        return VK_FORMAT_R32G32B32_SINT;
        break;
    case Nexus::Graphics::ShaderDataType::Int4:
        return VK_FORMAT_R32G32B32A32_SINT;
        break;

    case Nexus::Graphics::ShaderDataType::SignedByte:
        return VK_FORMAT_R8_SINT;
        break;
    case Nexus::Graphics::ShaderDataType::SignedByte2:
        return VK_FORMAT_R8G8_SINT;
        break;
    case Nexus::Graphics::ShaderDataType::SignedByte4:
        return VK_FORMAT_R8G8B8A8_SINT;
        break;

    case Nexus::Graphics::ShaderDataType::SignedByteNormalized:
        return VK_FORMAT_R8_SNORM;
        break;
    case Nexus::Graphics::ShaderDataType::SignedByte2Normalized:
        return VK_FORMAT_R8G8_SNORM;
        break;
    case Nexus::Graphics::ShaderDataType::SignedByte4Normalized:
        return VK_FORMAT_R8G8B8A8_SNORM;
        break;

    case Nexus::Graphics::ShaderDataType::Short:
        return VK_FORMAT_R16_SINT;
        break;
    case Nexus::Graphics::ShaderDataType::Short2:
        return VK_FORMAT_R16G16_SINT;
        break;
    case Nexus::Graphics::ShaderDataType::Short4:
        return VK_FORMAT_R16G16B16A16_SINT;
        break;

    case Nexus::Graphics::ShaderDataType::ShortNormalized:
        return VK_FORMAT_R16_SNORM;
        break;
    case Nexus::Graphics::ShaderDataType::Short2Normalized:
        return VK_FORMAT_R16G16_SNORM;
        break;
    case Nexus::Graphics::ShaderDataType::Short4Normalized:
        return VK_FORMAT_R16G16B16A16_SNORM;
        break;

    case Nexus::Graphics::ShaderDataType::UInt:
        return VK_FORMAT_R32_UINT;
        break;
    case Nexus::Graphics::ShaderDataType::UInt2:
        return VK_FORMAT_R32G32_UINT;
        break;
    case Nexus::Graphics::ShaderDataType::UInt3:
        return VK_FORMAT_R32G32B32_UINT;
        break;
    case Nexus::Graphics::ShaderDataType::UInt4:
        return VK_FORMAT_R32G32B32A32_UINT;
        break;

    case Nexus::Graphics::ShaderDataType::UShort:
        return VK_FORMAT_R16_UINT;
        break;
    case Nexus::Graphics::ShaderDataType::UShort2:
        return VK_FORMAT_R16G16_UINT;
        break;
    case Nexus::Graphics::ShaderDataType::UShort4:
        return VK_FORMAT_R16G16B16A16_UINT;
        break;

    case Nexus::Graphics::ShaderDataType::UShortNormalized:
        return VK_FORMAT_R16_UNORM;
        break;
    case Nexus::Graphics::ShaderDataType::UShort2Normalized:
        return VK_FORMAT_R16G16B16_UNORM;
        break;
    case Nexus::Graphics::ShaderDataType::UShort4Normalized:
        return VK_FORMAT_R16G16B16A16_UNORM;
        break;
    default:
        throw std::runtime_error("Failed to find valid vertex buffer element type");
    }
}

VkSampleCountFlagBits GetVkSampleCount(Nexus::Graphics::MultiSamples samples)
{
    switch (samples)
    {
    case Nexus::Graphics::MultiSamples::SampleCount1:
        return VK_SAMPLE_COUNT_1_BIT;
    case Nexus::Graphics::MultiSamples::SampleCount2:
        return VK_SAMPLE_COUNT_2_BIT;
    case Nexus::Graphics::MultiSamples::SampleCount4:
        return VK_SAMPLE_COUNT_4_BIT;
    case Nexus::Graphics::MultiSamples::SampleCount8:
        return VK_SAMPLE_COUNT_8_BIT;
    default:
        throw std::runtime_error("Failed to find a valid sample count");
    }
}

#endif