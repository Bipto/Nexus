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

void GetVkFilterFromNexusFormat(Nexus::Graphics::SamplerFilter filter, VkFilter &min, VkFilter &max, VkSamplerMipmapMode &mipmapMode)
{
    switch (filter)
    {
    case Nexus::Graphics::SamplerFilter::Anisotropic:
        min = VK_FILTER_LINEAR;
        max = VK_FILTER_LINEAR;
        mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        break;

    case Nexus::Graphics::SamplerFilter::MinPoint_MagPoint_MipPoint:
        min = VK_FILTER_NEAREST;
        max = VK_FILTER_NEAREST;
        mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        break;

    case Nexus::Graphics::SamplerFilter::MinPoint_MagPoint_MipLinear:
        min = VK_FILTER_NEAREST;
        max = VK_FILTER_NEAREST;
        mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        break;

    case Nexus::Graphics::SamplerFilter::MinPoint_MagLinear_MipPoint:
        min = VK_FILTER_NEAREST;
        max = VK_FILTER_LINEAR;
        mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        break;

    case Nexus::Graphics::SamplerFilter::MinPoint_MagLinear_MipLinear:
        min = VK_FILTER_NEAREST;
        max = VK_FILTER_LINEAR;
        mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        break;

    case Nexus::Graphics::SamplerFilter::MinLinear_MagPoint_MipPoint:
        min = VK_FILTER_LINEAR;
        max = VK_FILTER_NEAREST;
        mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        break;

    case Nexus::Graphics::SamplerFilter::MinLinear_MagPoint_MipLinear:
        min = VK_FILTER_LINEAR;
        max = VK_FILTER_NEAREST;
        mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        break;

    case Nexus::Graphics::SamplerFilter::MinLinear_MagLinear_MipPoint:
        min = VK_FILTER_LINEAR;
        max = VK_FILTER_LINEAR;
        mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        break;

    case Nexus::Graphics::SamplerFilter::MinLinear_MagLinear_MipLinear:
        min = VK_FILTER_LINEAR;
        max = VK_FILTER_LINEAR;
        mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        break;

    default:
        throw std::runtime_error("Failed to find a valid sampler filter");
    }
}

VkSamplerAddressMode GetVkSamplerAddressMode(Nexus::Graphics::SamplerAddressMode addressMode)
{
    switch (addressMode)
    {
    case Nexus::Graphics::SamplerAddressMode::Border:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    case Nexus::Graphics::SamplerAddressMode::Clamp:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case Nexus::Graphics::SamplerAddressMode::Mirror:
        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    case Nexus::Graphics::SamplerAddressMode::MirrorOnce:
        return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
    case Nexus::Graphics::SamplerAddressMode::Wrap:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    default:
        throw std::runtime_error("Failed to find valid sampler address mode");
    }
}

VkCompareOp GetCompareOp(Nexus::Graphics::ComparisonFunction function)
{
    switch (function)
    {
    case Nexus::Graphics::ComparisonFunction::Always:
        return VK_COMPARE_OP_ALWAYS;
    case Nexus::Graphics::ComparisonFunction::Equal:
        return VK_COMPARE_OP_EQUAL;
    case Nexus::Graphics::ComparisonFunction::Greater:
        return VK_COMPARE_OP_GREATER;
    case Nexus::Graphics::ComparisonFunction::GreaterEqual:
        return VK_COMPARE_OP_GREATER_OR_EQUAL;
    case Nexus::Graphics::ComparisonFunction::Less:
        return VK_COMPARE_OP_LESS;
    case Nexus::Graphics::ComparisonFunction::LessEqual:
        return VK_COMPARE_OP_LESS_OR_EQUAL;
    case Nexus::Graphics::ComparisonFunction::Never:
        return VK_COMPARE_OP_NEVER;
    case Nexus::Graphics::ComparisonFunction::NotEqual:
        return VK_COMPARE_OP_NOT_EQUAL;
    default:
        throw std::runtime_error("Failed to find a valid comparison function");
    }
}

VkBlendOp GetVkBlendOp(Nexus::Graphics::BlendEquation function)
{
    switch (function)
    {
    case Nexus::Graphics::BlendEquation::Add:
        return VK_BLEND_OP_ADD;
    case Nexus::Graphics::BlendEquation::Subtract:
        return VK_BLEND_OP_SUBTRACT;
    case Nexus::Graphics::BlendEquation::ReverseSubtract:
        return VK_BLEND_OP_REVERSE_SUBTRACT;
    case Nexus::Graphics::BlendEquation::Min:
        return VK_BLEND_OP_MIN;
    case Nexus::Graphics::BlendEquation::Max:
        return VK_BLEND_OP_MAX;
    default:
        throw std::runtime_error("Failed to find a valid blend operation");
    }
}

VkBlendFactor GetVkBlendFactor(Nexus::Graphics::BlendFunction function)
{
    switch (function)
    {
    case Nexus::Graphics::BlendFunction::Zero:
        return VK_BLEND_FACTOR_ZERO;
    case Nexus::Graphics::BlendFunction::One:
        return VK_BLEND_FACTOR_ONE;
    case Nexus::Graphics::BlendFunction::SourceColor:
        return VK_BLEND_FACTOR_SRC_COLOR;
    case Nexus::Graphics::BlendFunction::OneMinusSourceColor:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
    case Nexus::Graphics::BlendFunction::DestinationColor:
        return VK_BLEND_FACTOR_DST_COLOR;
    case Nexus::Graphics::BlendFunction::OneMinusDestinationColor:
        return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
    case Nexus::Graphics::BlendFunction::SourceAlpha:
        return VK_BLEND_FACTOR_SRC_ALPHA;
    case Nexus::Graphics::BlendFunction::OneMinusSourceAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    case Nexus::Graphics::BlendFunction::DestinationAlpha:
        return VK_BLEND_FACTOR_DST_ALPHA;
    case Nexus::Graphics::BlendFunction::OneMinusDestinationAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    default:
        throw std::runtime_error("Failed to find a valid blend factor");
    }
}

VkBorderColor GetVkBorderColor(Nexus::Graphics::BorderColor color)
{
    switch (color)
    {
    case Nexus::Graphics::BorderColor::TransparentBlack:
        return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    case Nexus::Graphics::BorderColor::OpaqueBlack:
        return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
    case Nexus::Graphics::BorderColor::OpaqueWhite:
        return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    default:
        throw std::runtime_error("Failed to find a valid border color");
    }
}

#endif