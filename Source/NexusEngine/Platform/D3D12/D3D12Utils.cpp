#include "D3D12Utils.hpp"

#if defined(NX_PLATFORM_D3D12)

DXGI_FORMAT GetD3D12TextureFormat(Nexus::Graphics::TextureFormat format)
{
    switch (format)
    {
    case Nexus::Graphics::TextureFormat::RGBA8:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case Nexus::Graphics::TextureFormat::R8:
        return DXGI_FORMAT_R8_UNORM;
    default:
        throw std::runtime_error("Failed to find a valid format");
    }
}

DXGI_FORMAT GetD3D12DepthFormat(Nexus::Graphics::DepthFormat format)
{
    switch (format)
    {
    case Nexus::Graphics::DepthFormat::DEPTH24STENCIL8:
        return DXGI_FORMAT_D24_UNORM_S8_UINT;
    default:
        throw std::runtime_error("Failed to find a valid format");
    }
}

D3D12_CULL_MODE GetCullMode(Nexus::Graphics::CullMode cullMode)
{
    switch (cullMode)
    {
    case Nexus::Graphics::CullMode::Back:
        return D3D12_CULL_MODE_BACK;
    case Nexus::Graphics::CullMode::Front:
        return D3D12_CULL_MODE_FRONT;
    case Nexus::Graphics::CullMode::None:
        return D3D12_CULL_MODE_NONE;
    default:
        throw std::runtime_error("Failed to find a valid format");
    }
}

D3D12_COMPARISON_FUNC GetComparisonFunction(Nexus::Graphics::ComparisonFunction function)
{
    switch (function)
    {
    case Nexus::Graphics::ComparisonFunction::Always:
        return D3D12_COMPARISON_FUNC_ALWAYS;
    case Nexus::Graphics::ComparisonFunction::Equal:
        return D3D12_COMPARISON_FUNC_EQUAL;
    case Nexus::Graphics::ComparisonFunction::Greater:
        return D3D12_COMPARISON_FUNC_GREATER;
    case Nexus::Graphics::ComparisonFunction::GreaterEqual:
        return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
    case Nexus::Graphics::ComparisonFunction::Less:
        return D3D12_COMPARISON_FUNC_LESS;
    case Nexus::Graphics::ComparisonFunction::LessEqual:
        return D3D12_COMPARISON_FUNC_LESS_EQUAL;
    case Nexus::Graphics::ComparisonFunction::Never:
        return D3D12_COMPARISON_FUNC_NEVER;
    case Nexus::Graphics::ComparisonFunction::NotEqual:
        return D3D12_COMPARISON_FUNC_NOT_EQUAL;
    default:
        throw std::runtime_error("Failed to find a valid format");
    }
}

D3D12_STENCIL_OP GetStencilOperation(Nexus::Graphics::StencilOperation operation)
{
    switch (operation)
    {
    case Nexus::Graphics::StencilOperation::Keep:
        return D3D12_STENCIL_OP_KEEP;
    case Nexus::Graphics::StencilOperation::Zero:
        return D3D12_STENCIL_OP_ZERO;
    case Nexus::Graphics::StencilOperation::Replace:
        return D3D12_STENCIL_OP_REPLACE;
    case Nexus::Graphics::StencilOperation::Increment:
        return D3D12_STENCIL_OP_INCR;
    case Nexus::Graphics::StencilOperation::Decrement:
        return D3D12_STENCIL_OP_DECR;
    case Nexus::Graphics::StencilOperation::Invert:
        return D3D12_STENCIL_OP_INVERT;
    }
}

D3D12_FILL_MODE GetFillMode(Nexus::Graphics::FillMode fillMode)
{
    switch (fillMode)
    {
    case Nexus::Graphics::FillMode::Solid:
        return D3D12_FILL_MODE_SOLID;
    case Nexus::Graphics::FillMode::Wireframe:
        return D3D12_FILL_MODE_WIREFRAME;
    }
}

D3D12_BLEND GetBlendFunction(Nexus::Graphics::BlendFunction function)
{
    switch (function)
    {
    case Nexus::Graphics::BlendFunction::Zero:
        return D3D12_BLEND_ZERO;
    case Nexus::Graphics::BlendFunction::One:
        return D3D12_BLEND_ONE;
    case Nexus::Graphics::BlendFunction::SourceColor:
        return D3D12_BLEND_SRC_COLOR;
    case Nexus::Graphics::BlendFunction::OneMinusSourceColor:
        return D3D12_BLEND_INV_SRC_COLOR;
    case Nexus::Graphics::BlendFunction::DestinationColor:
        return D3D12_BLEND_DEST_COLOR;
    case Nexus::Graphics::BlendFunction::OneMinusDestinationColor:
        return D3D12_BLEND_INV_DEST_COLOR;
    case Nexus::Graphics::BlendFunction::SourceAlpha:
        return D3D12_BLEND_SRC_ALPHA;
    case Nexus::Graphics::BlendFunction::OneMinusSourceAlpha:
        return D3D12_BLEND_INV_SRC_ALPHA;
    case Nexus::Graphics::BlendFunction::DestinationAlpha:
        return D3D12_BLEND_DEST_ALPHA;
    case Nexus::Graphics::BlendFunction::OneMinusDestinationAlpha:
        return D3D12_BLEND_INV_DEST_ALPHA;
    default:
        throw std::runtime_error("Failed to find a valid blend function");
    }
}

D3D12_BLEND_OP GetBlendEquation(Nexus::Graphics::BlendEquation equation)
{
    switch (equation)
    {
    case Nexus::Graphics::BlendEquation::Add:
        return D3D12_BLEND_OP_ADD;
    case Nexus::Graphics::BlendEquation::Subtract:
        return D3D12_BLEND_OP_SUBTRACT;
    case Nexus::Graphics::BlendEquation::ReverseSubtract:
        return D3D12_BLEND_OP_REV_SUBTRACT;
    case Nexus::Graphics::BlendEquation::Min:
        return D3D12_BLEND_OP_MIN;
    case Nexus::Graphics::BlendEquation::Max:
        return D3D12_BLEND_OP_MAX;
    default:
        throw std::runtime_error("Failed to find a valid blend operation");
    }
}

DXGI_FORMAT GetD3D12BaseType(const Nexus::Graphics::VertexBufferElement &element)
{
    switch (element.Type)
    {
    case Nexus::Graphics::ShaderDataType::Byte:
        return DXGI_FORMAT_R8_UINT;
        break;
    case Nexus::Graphics::ShaderDataType::Byte2:
        return DXGI_FORMAT_R8G8_UINT;
        break;
    case Nexus::Graphics::ShaderDataType::Byte4:
        return DXGI_FORMAT_R8G8B8A8_UINT;
        break;

    case Nexus::Graphics::ShaderDataType::NormByte:
        return DXGI_FORMAT_R8_UNORM;
        break;
    case Nexus::Graphics::ShaderDataType::NormByte2:
        return DXGI_FORMAT_R8G8_UNORM;
        break;
    case Nexus::Graphics::ShaderDataType::NormByte4:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
        break;

    case Nexus::Graphics::ShaderDataType::Float:
        return DXGI_FORMAT_R32_FLOAT;
        break;
    case Nexus::Graphics::ShaderDataType::Float2:
        return DXGI_FORMAT_R32G32_FLOAT;
        break;
    case Nexus::Graphics::ShaderDataType::Float3:
        return DXGI_FORMAT_R32G32B32_FLOAT;
        break;
    case Nexus::Graphics::ShaderDataType::Float4:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
        break;

    case Nexus::Graphics::ShaderDataType::Half:
        return DXGI_FORMAT_R16_FLOAT;
        break;
    case Nexus::Graphics::ShaderDataType::Half2:
        return DXGI_FORMAT_R16G16_FLOAT;
        break;
    case Nexus::Graphics::ShaderDataType::Half4:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;
        break;

    case Nexus::Graphics::ShaderDataType::Int:
        return DXGI_FORMAT_R32_SINT;
        break;
    case Nexus::Graphics::ShaderDataType::Int2:
        return DXGI_FORMAT_R32G32_SINT;
        break;
    case Nexus::Graphics::ShaderDataType::Int3:
        return DXGI_FORMAT_R32G32B32_SINT;
        break;
    case Nexus::Graphics::ShaderDataType::Int4:
        return DXGI_FORMAT_R32G32B32A32_SINT;
        break;

    case Nexus::Graphics::ShaderDataType::SignedByte:
        return DXGI_FORMAT_R8_SNORM;
        break;
    case Nexus::Graphics::ShaderDataType::SignedByte2:
        return DXGI_FORMAT_R8G8_SNORM;
        break;
    case Nexus::Graphics::ShaderDataType::SignedByte4:
        return DXGI_FORMAT_R8G8B8A8_SNORM;
        break;

    case Nexus::Graphics::ShaderDataType::SignedByteNormalized:
        return DXGI_FORMAT_R8_SNORM;
        break;
    case Nexus::Graphics::ShaderDataType::SignedByte2Normalized:
        return DXGI_FORMAT_R8G8_SNORM;
        break;
    case Nexus::Graphics::ShaderDataType::SignedByte4Normalized:
        return DXGI_FORMAT_R8G8B8A8_SNORM;
        break;

    case Nexus::Graphics::ShaderDataType::Short:
        return DXGI_FORMAT_R16_UINT;
        break;
    case Nexus::Graphics::ShaderDataType::Short2:
        return DXGI_FORMAT_R16G16_UINT;
        break;
    case Nexus::Graphics::ShaderDataType::Short4:
        return DXGI_FORMAT_R16G16B16A16_UINT;
        break;

    case Nexus::Graphics::ShaderDataType::ShortNormalized:
        return DXGI_FORMAT_R16_SNORM;
        break;
    case Nexus::Graphics::ShaderDataType::Short2Normalized:
        return DXGI_FORMAT_R16G16_SNORM;
        break;
    case Nexus::Graphics::ShaderDataType::Short4Normalized:
        return DXGI_FORMAT_R16G16B16A16_SNORM;
        break;

    case Nexus::Graphics::ShaderDataType::UInt:
        return DXGI_FORMAT_R32_UINT;
        break;
    case Nexus::Graphics::ShaderDataType::UInt2:
        return DXGI_FORMAT_R32G32_UINT;
        break;
    case Nexus::Graphics::ShaderDataType::UInt3:
        return DXGI_FORMAT_R32G32B32_UINT;
        break;
    case Nexus::Graphics::ShaderDataType::UInt4:
        return DXGI_FORMAT_R32G32B32A32_UINT;
        break;

    case Nexus::Graphics::ShaderDataType::UShort:
        return DXGI_FORMAT_R16_UINT;
        break;
    case Nexus::Graphics::ShaderDataType::UShort2:
        return DXGI_FORMAT_R16G16_UINT;
        break;
    case Nexus::Graphics::ShaderDataType::UShort4:
        return DXGI_FORMAT_R16G16B16A16_UINT;
        break;

    case Nexus::Graphics::ShaderDataType::UShortNormalized:
        return DXGI_FORMAT_R16_UNORM;
        break;
    case Nexus::Graphics::ShaderDataType::UShort2Normalized:
        return DXGI_FORMAT_R16G16_UNORM;
        break;
    case Nexus::Graphics::ShaderDataType::UShort4Normalized:
        return DXGI_FORMAT_R16G16B16A16_UNORM;
        break;
    default:
        throw std::runtime_error("Failed to find valid vertex buffer element type");
    }
}

D3D12_FILTER GetD3D12Filter(Nexus::Graphics::SamplerFilter filter)
{
    switch (filter)
    {
    case Nexus::Graphics::SamplerFilter::Anisotropic:
        D3D12_FILTER_ANISOTROPIC;
        break;
    case Nexus::Graphics::SamplerFilter::MinPoint_MagPoint_MipPoint:
        return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        break;
    case Nexus::Graphics::SamplerFilter::MinPoint_MagPoint_MipLinear:
        return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
        break;
    case Nexus::Graphics::SamplerFilter::MinPoint_MagLinear_MipPoint:
        return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
        break;
    case Nexus::Graphics::SamplerFilter::MinPoint_MagLinear_MipLinear:
        return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
        break;
    case Nexus::Graphics::SamplerFilter::MinLinear_MagPoint_MipPoint:
        return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
        break;
    case Nexus::Graphics::SamplerFilter::MinLinear_MagPoint_MipLinear:
        return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        break;
    case Nexus::Graphics::SamplerFilter::MinLinear_MagLinear_MipPoint:
        return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        break;
    case Nexus::Graphics::SamplerFilter::MinLinear_MagLinear_MipLinear:
        return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        break;

    default:
        throw std::runtime_error("Failed to find a valid sampler filter");
    }
}

D3D12_TEXTURE_ADDRESS_MODE GetD3D12TextureAddressMode(Nexus::Graphics::SamplerAddressMode addressMode)
{
    switch (addressMode)
    {
    case Nexus::Graphics::SamplerAddressMode::Border:
        return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        break;
    case Nexus::Graphics::SamplerAddressMode::Clamp:
        return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        break;
    case Nexus::Graphics::SamplerAddressMode::Mirror:
        return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        break;
    case Nexus::Graphics::SamplerAddressMode::MirrorOnce:
        return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
        break;
    case Nexus::Graphics::SamplerAddressMode::Wrap:
        return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        break;
    default:
        throw std::runtime_error("Failed to find a valid sampler address mode");
    }
}

#endif