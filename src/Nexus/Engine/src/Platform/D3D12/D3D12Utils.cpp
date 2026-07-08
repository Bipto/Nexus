#include "D3D12Utils.hpp"

#if defined(NX_PLATFORM_D3D12)

#include "GraphicsDeviceD3D12.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "PipelineD3D12.hpp"
#include "ShaderModuleD3D12.hpp"
#include "StreamStateBuilder.hpp"
#include "TextureD3D12.hpp"

namespace Nexus::D3D12
{
    DXGI_FORMAT GetD3D12PixelFormat(Nexus::Graphics::PixelFormat format)
    {
        switch (format)
        {
        case Nexus::Graphics::PixelFormat::R8_UNorm:
            return DXGI_FORMAT_R8_UNORM;
        case Nexus::Graphics::PixelFormat::R8_SNorm:
            return DXGI_FORMAT_R8_SNORM;
        case Nexus::Graphics::PixelFormat::R8_UInt:
            return DXGI_FORMAT_R8_UINT;
        case Nexus::Graphics::PixelFormat::R8_SInt:
            return DXGI_FORMAT_R8_SINT;

        case Nexus::Graphics::PixelFormat::R16_UNorm:
            return DXGI_FORMAT_R16_UNORM;
        case Nexus::Graphics::PixelFormat::R16_SNorm:
            return DXGI_FORMAT_R16_SNORM;
        case Nexus::Graphics::PixelFormat::R16_UInt:
            return DXGI_FORMAT_R16_UINT;
        case Nexus::Graphics::PixelFormat::R16_SInt:
            return DXGI_FORMAT_R16_SINT;
        case Nexus::Graphics::PixelFormat::R16_Float:
            return DXGI_FORMAT_R16_FLOAT;

        case Nexus::Graphics::PixelFormat::R32_UInt:
            return DXGI_FORMAT_R32_UINT;
        case Nexus::Graphics::PixelFormat::R32_SInt:
            return DXGI_FORMAT_R32_SINT;
        case Nexus::Graphics::PixelFormat::R32_Float:
            return DXGI_FORMAT_R32_FLOAT;

        case Nexus::Graphics::PixelFormat::R8_G8_UNorm:
            return DXGI_FORMAT_R8G8_UNORM;
        case Nexus::Graphics::PixelFormat::R8_G8_SNorm:
            return DXGI_FORMAT_R8G8_SNORM;
        case Nexus::Graphics::PixelFormat::R8_G8_UInt:
            return DXGI_FORMAT_R8G8_UINT;
        case Nexus::Graphics::PixelFormat::R8_G8_SInt:
            return DXGI_FORMAT_R8G8_SINT;

        case Nexus::Graphics::PixelFormat::R16_G16_UNorm:
            return DXGI_FORMAT_R16G16_UNORM;
        case Nexus::Graphics::PixelFormat::R16_G16_SNorm:
            return DXGI_FORMAT_R16G16_SNORM;
        case Nexus::Graphics::PixelFormat::R16_G16_UInt:
            return DXGI_FORMAT_R16G16_UINT;
        case Nexus::Graphics::PixelFormat::R16_G16_SInt:
            return DXGI_FORMAT_R16G16_SINT;
        case Nexus::Graphics::PixelFormat::R16_G16_Float:
            return DXGI_FORMAT_R16G16_FLOAT;

        case Nexus::Graphics::PixelFormat::R32_G32_UInt:
            return DXGI_FORMAT_R32G32_UINT;
        case Nexus::Graphics::PixelFormat::R32_G32_SInt:
            return DXGI_FORMAT_R32G32_SINT;
        case Nexus::Graphics::PixelFormat::R32_G32_Float:
            return DXGI_FORMAT_R32G32_FLOAT;

        case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm_SRGB:
            return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case Nexus::Graphics::PixelFormat::B8_G8_R8_A8_UNorm:
            return DXGI_FORMAT_B8G8R8A8_UNORM;
        case Nexus::Graphics::PixelFormat::B8_G8_R8_A8_UNorm_SRGB:
            return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_SNorm:
            return DXGI_FORMAT_R8G8B8A8_SNORM;
        case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UInt:
            return DXGI_FORMAT_R8G8B8A8_UINT;
        case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_SInt:
            return DXGI_FORMAT_R8G8B8A8_SINT;

        case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_UNorm:
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_SNorm:
            return DXGI_FORMAT_R16G16B16A16_SNORM;
        case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_UInt:
            return DXGI_FORMAT_R16G16B16A16_UINT;
        case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_SInt:
            return DXGI_FORMAT_R16G16B16A16_SINT;
        case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_Float:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;

        case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_UInt:
            return DXGI_FORMAT_R32G32B32A32_UINT;
        case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_SInt:
            return DXGI_FORMAT_R32G32B32A32_SINT;
        case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_Float:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;

        case Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UNorm:
            return DXGI_FORMAT_R10G10B10A2_UNORM;
        case Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UInt:
            return DXGI_FORMAT_R10G10B10A2_UINT;
        case Nexus::Graphics::PixelFormat::R11_G11_B10_Float:
            return DXGI_FORMAT_R11G11B10_FLOAT;

        case Nexus::Graphics::PixelFormat::BC1_Rgb_UNorm:
        case Nexus::Graphics::PixelFormat::BC1_Rgba_UNorm:
            return DXGI_FORMAT_BC1_UNORM;
        case Nexus::Graphics::PixelFormat::BC1_Rgb_UNorm_SRgb:
        case Nexus::Graphics::PixelFormat::BC1_Rgba_UNorm_SRgb:
            return DXGI_FORMAT_BC1_UNORM_SRGB;
        case Nexus::Graphics::PixelFormat::BC2_UNorm:
            return DXGI_FORMAT_BC2_UNORM;
        case Nexus::Graphics::PixelFormat::BC2_UNorm_SRgb:
            return DXGI_FORMAT_BC2_UNORM_SRGB;
        case Nexus::Graphics::PixelFormat::BC3_UNorm:
            return DXGI_FORMAT_BC3_UNORM;
        case Nexus::Graphics::PixelFormat::BC3_UNorm_SRgb:
            return DXGI_FORMAT_BC3_UNORM_SRGB;
        case Nexus::Graphics::PixelFormat::BC4_UNorm:
            return DXGI_FORMAT_BC4_UNORM;
        case Nexus::Graphics::PixelFormat::BC4_SNorm:
            return DXGI_FORMAT_BC4_SNORM;
        case Nexus::Graphics::PixelFormat::BC5_UNorm:
            return DXGI_FORMAT_BC5_UNORM;
        case Nexus::Graphics::PixelFormat::BC5_SNorm:
            return DXGI_FORMAT_BC5_SNORM;
        case Nexus::Graphics::PixelFormat::BC7_UNorm:
            return DXGI_FORMAT_BC7_UNORM;
        case Nexus::Graphics::PixelFormat::BC7_UNorm_SRgb:
            return DXGI_FORMAT_BC7_UNORM_SRGB;

        case Nexus::Graphics::PixelFormat::D16_UNorm:
            return DXGI_FORMAT_D16_UNORM;
        case Nexus::Graphics::PixelFormat::D24_UNorm_S8_UInt:
            return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case Nexus::Graphics::PixelFormat::D32_SFloat:
            return DXGI_FORMAT_D32_FLOAT;
        case Nexus::Graphics::PixelFormat::D32_SFloat_S8_UInt:
            return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

        default:
            throw std::runtime_error("Failed to find a valid format");
        }
    }

    D3D12_TEXTURE_LAYOUT GetTextureLayout(Graphics::TextureTiling tiling)
    {
        switch (tiling)
        {
        case Graphics::TextureTiling::Optimal:
            return D3D12_TEXTURE_LAYOUT_UNKNOWN;
        case Graphics::TextureTiling::Linear:
            return D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        default:
            throw std::runtime_error("Failed to find a valid texture tiling");
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
        case Nexus::Graphics::CullMode::CullNone:
            return D3D12_CULL_MODE_NONE;
        default:
            throw std::runtime_error("Failed to find a valid format");
        }
    }

    D3D12_COMPARISON_FUNC
    GetComparisonFunction(Nexus::Graphics::ComparisonFunction function)
    {
        switch (function)
        {
        case Nexus::Graphics::ComparisonFunction::AlwaysPass:
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

    D3D12_STENCIL_OP
    GetStencilOperation(Nexus::Graphics::StencilOperation operation)
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
        default:
            throw std::runtime_error("Invalid stencil operation entered");
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
        default:
            throw std::runtime_error("Invalid fill mode entered");
        }
    }

    D3D12_BLEND GetBlendFunction(Nexus::Graphics::BlendFactor function)
    {
        switch (function)
        {
        case Nexus::Graphics::BlendFactor::Zero:
            return D3D12_BLEND_ZERO;
        case Nexus::Graphics::BlendFactor::One:
            return D3D12_BLEND_ONE;
        case Nexus::Graphics::BlendFactor::SourceColour:
            return D3D12_BLEND_SRC_COLOR;
        case Nexus::Graphics::BlendFactor::OneMinusSourceColour:
            return D3D12_BLEND_INV_SRC_COLOR;
        case Nexus::Graphics::BlendFactor::DestinationColour:
            return D3D12_BLEND_DEST_COLOR;
        case Nexus::Graphics::BlendFactor::OneMinusDestinationColour:
            return D3D12_BLEND_INV_DEST_COLOR;
        case Nexus::Graphics::BlendFactor::SourceAlpha:
            return D3D12_BLEND_SRC_ALPHA;
        case Nexus::Graphics::BlendFactor::OneMinusSourceAlpha:
            return D3D12_BLEND_INV_SRC_ALPHA;
        case Nexus::Graphics::BlendFactor::DestinationAlpha:
            return D3D12_BLEND_DEST_ALPHA;
        case Nexus::Graphics::BlendFactor::OneMinusDestinationAlpha:
            return D3D12_BLEND_INV_DEST_ALPHA;
        case Nexus::Graphics::BlendFactor::FactorColour:
            return D3D12_BLEND_BLEND_FACTOR;
        case Nexus::Graphics::BlendFactor::OneMinusFactorColour:
            return D3D12_BLEND_INV_BLEND_FACTOR;
        case Nexus::Graphics::BlendFactor::FactorAlpha:
            return D3D12_BLEND_ALPHA_FACTOR;
        case Nexus::Graphics::BlendFactor::OneMinusFactorAlpha:
            return D3D12_BLEND_INV_ALPHA_FACTOR;
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

    DXGI_FORMAT
    GetD3D12BaseType(const Nexus::Graphics::VertexBufferElement &element)
    {
        switch (element.Type)
        {
        case Nexus::Graphics::ShaderDataType::R8_UInt:
            return DXGI_FORMAT_R8_UINT;
        case Nexus::Graphics::ShaderDataType::R8G8_UInt:
            return DXGI_FORMAT_R8G8_UINT;
        case Nexus::Graphics::ShaderDataType::R8G8B8A8_UInt:
            return DXGI_FORMAT_R8G8B8A8_UINT;

        case Nexus::Graphics::ShaderDataType::R8_UNorm:
            return DXGI_FORMAT_R8_UNORM;
        case Nexus::Graphics::ShaderDataType::R8G8_UNorm:
            return DXGI_FORMAT_R8G8_UNORM;
        case Nexus::Graphics::ShaderDataType::R8G8B8A8_UNorm:
            return DXGI_FORMAT_R8G8B8A8_UNORM;

        case Nexus::Graphics::ShaderDataType::R32_SFloat:
            return DXGI_FORMAT_R32_FLOAT;
        case Nexus::Graphics::ShaderDataType::R32G32_SFloat:
            return DXGI_FORMAT_R32G32_FLOAT;
        case Nexus::Graphics::ShaderDataType::R32G32B32_SFloat:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case Nexus::Graphics::ShaderDataType::R32G32B32A32_SFloat:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;

        case Nexus::Graphics::ShaderDataType::R16_SFloat:
            return DXGI_FORMAT_R16_FLOAT;
        case Nexus::Graphics::ShaderDataType::R16G16_SFloat:
            return DXGI_FORMAT_R16G16_FLOAT;
        case Nexus::Graphics::ShaderDataType::R16G16B16A16_SFloat:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;

        case Nexus::Graphics::ShaderDataType::R32_SInt:
            return DXGI_FORMAT_R32_SINT;
        case Nexus::Graphics::ShaderDataType::R32G32_SInt:
            return DXGI_FORMAT_R32G32_SINT;
        case Nexus::Graphics::ShaderDataType::R32G32B32_SInt:
            return DXGI_FORMAT_R32G32B32_SINT;
        case Nexus::Graphics::ShaderDataType::R32G32B32A32_SInt:
            return DXGI_FORMAT_R32G32B32A32_SINT;

        case Nexus::Graphics::ShaderDataType::R8_SInt:
            return DXGI_FORMAT_R8_SNORM;
        case Nexus::Graphics::ShaderDataType::R8G8_SInt:
            return DXGI_FORMAT_R8G8_SNORM;
        case Nexus::Graphics::ShaderDataType::R8G8B8A8_SInt:
            return DXGI_FORMAT_R8G8B8A8_SNORM;

        case Nexus::Graphics::ShaderDataType::R8_SNorm:
            return DXGI_FORMAT_R8_SNORM;
        case Nexus::Graphics::ShaderDataType::R8G8_SNorm:
            return DXGI_FORMAT_R8G8_SNORM;
        case Nexus::Graphics::ShaderDataType::R8G8B8A8_SNorm:
            return DXGI_FORMAT_R8G8B8A8_SNORM;

        case Nexus::Graphics::ShaderDataType::R16_SInt:
            return DXGI_FORMAT_R16_UINT;
        case Nexus::Graphics::ShaderDataType::R16G16_SInt:
            return DXGI_FORMAT_R16G16_UINT;
        case Nexus::Graphics::ShaderDataType::R16G16B16A16_SInt:
            return DXGI_FORMAT_R16G16B16A16_UINT;

        case Nexus::Graphics::ShaderDataType::R16_SNorm:
            return DXGI_FORMAT_R16_SNORM;
        case Nexus::Graphics::ShaderDataType::R16G16_SNorm:
            return DXGI_FORMAT_R16G16_SNORM;
        case Nexus::Graphics::ShaderDataType::R16G16B16A16_SNorm:
            return DXGI_FORMAT_R16G16B16A16_SNORM;

        case Nexus::Graphics::ShaderDataType::R32_UInt:
            return DXGI_FORMAT_R32_UINT;
        case Nexus::Graphics::ShaderDataType::R32G32_UInt:
            return DXGI_FORMAT_R32G32_UINT;
        case Nexus::Graphics::ShaderDataType::R32G32B32_UInt:
            return DXGI_FORMAT_R32G32B32_UINT;
        case Nexus::Graphics::ShaderDataType::R32G32B32A32_UInt:
            return DXGI_FORMAT_R32G32B32A32_UINT;

        case Nexus::Graphics::ShaderDataType::R16_UInt:
            return DXGI_FORMAT_R16_UINT;
        case Nexus::Graphics::ShaderDataType::R16G16_UInt:
            return DXGI_FORMAT_R16G16_UINT;
        case Nexus::Graphics::ShaderDataType::R16G16B16A16_UInt:
            return DXGI_FORMAT_R16G16B16A16_UINT;

        case Nexus::Graphics::ShaderDataType::R16_UNorm:
            return DXGI_FORMAT_R16_UNORM;
        case Nexus::Graphics::ShaderDataType::R16G16_UNorm:
            return DXGI_FORMAT_R16G16_UNORM;
        case Nexus::Graphics::ShaderDataType::R16G16B16A16_UNorm:
            return DXGI_FORMAT_R16G16B16A16_UNORM;

        case Nexus::Graphics::ShaderDataType::A2B10G10R10_UInt:
            return DXGI_FORMAT_R10G10B10A2_UINT;
        case Nexus::Graphics::ShaderDataType::A2B10G10R10_UNorm:
            return DXGI_FORMAT_R10G10B10A2_UNORM;

        default:
            throw std::runtime_error("Failed to find valid vertex buffer element type");
        }
    }

    D3D12_FILTER GetD3D12Filter(Nexus::Graphics::SamplerFilter filter)
    {
        switch (filter)
        {
        case Nexus::Graphics::SamplerFilter::Anisotropic:
            return D3D12_FILTER_ANISOTROPIC;
        case Nexus::Graphics::SamplerFilter::MinPoint_MagPoint_MipPoint:
            return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        case Nexus::Graphics::SamplerFilter::MinPoint_MagPoint_MipLinear:
            return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
        case Nexus::Graphics::SamplerFilter::MinPoint_MagLinear_MipPoint:
            return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
        case Nexus::Graphics::SamplerFilter::MinPoint_MagLinear_MipLinear:
            return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
        case Nexus::Graphics::SamplerFilter::MinLinear_MagPoint_MipPoint:
            return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
        case Nexus::Graphics::SamplerFilter::MinLinear_MagPoint_MipLinear:
            return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        case Nexus::Graphics::SamplerFilter::MinLinear_MagLinear_MipPoint:
            return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        case Nexus::Graphics::SamplerFilter::MinLinear_MagLinear_MipLinear:
            return D3D12_FILTER_MIN_MAG_MIP_LINEAR;

        default:
            throw std::runtime_error("Failed to find a valid filter");
        }

        return {};
    }

    D3D12_TEXTURE_ADDRESS_MODE
    GetD3D12TextureAddressMode(Nexus::Graphics::SamplerAddressMode addressMode)
    {
        switch (addressMode)
        {
        case Nexus::Graphics::SamplerAddressMode::Border:
            return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        case Nexus::Graphics::SamplerAddressMode::Clamp:
            return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        case Nexus::Graphics::SamplerAddressMode::Mirror:
            return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        case Nexus::Graphics::SamplerAddressMode::MirrorOnce:
            return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
        case Nexus::Graphics::SamplerAddressMode::Wrap:
            return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        default:
            throw std::runtime_error("Failed to find a valid sampler address mode");
        }
    }

    DXGI_FORMAT
    GetD3D12IndexBufferFormat(Nexus::Graphics::IndexFormat format)
    {
        switch (format)
        {
        case Nexus::Graphics::IndexFormat::UInt16:
            return DXGI_FORMAT_R16_UINT;
        case Nexus::Graphics::IndexFormat::UInt32:
            return DXGI_FORMAT_R32_UINT;
        default:
            throw std::runtime_error("Invalid index buffer format entered");
        }
    }

    D3D12_PRIMITIVE_TOPOLOGY_TYPE
    GetPipelineTopology(Nexus::Graphics::Topology topology)
    {
        switch (topology)
        {
        case Nexus::Graphics::Topology::LineList:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        case Nexus::Graphics::Topology::LineStrip:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        case Nexus::Graphics::Topology::PointList:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
        case Nexus::Graphics::Topology::TriangleList:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        case Nexus::Graphics::Topology::TriangleStrip:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        default:
            throw std::runtime_error("Could not find a valid topology");
        }
    }

    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGraphicsPipelineTraditional(
        Graphics::GraphicsDeviceD3D12 *device, const Graphics::GraphicsPipelineDescription &description,
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature,
        const std::vector<D3D12_INPUT_ELEMENT_DESC> &inputLayout
    )
    {
        std::vector<DXGI_FORMAT> rtvFormats;

        for (uint32_t index = 0; index < description.ColourTargetCount; index++)
        {
            DXGI_FORMAT colourFormat = D3D12::GetD3D12PixelFormat(description.ColourFormats.at(index));
            rtvFormats.push_back(colourFormat);
        }

        D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
        pipelineDesc.pRootSignature = rootSignature.Get();
        pipelineDesc.InputLayout.NumElements = inputLayout.size();
        pipelineDesc.InputLayout.pInputElementDescs = inputLayout.data();
        pipelineDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

        // setup shaders
        pipelineDesc.VS.BytecodeLength = 0;
        pipelineDesc.VS.pShaderBytecode = nullptr;
        pipelineDesc.PS.BytecodeLength = 0;
        pipelineDesc.PS.pShaderBytecode = nullptr;
        pipelineDesc.DS.BytecodeLength = 0;
        pipelineDesc.DS.pShaderBytecode = nullptr;
        pipelineDesc.HS.BytecodeLength = 0;
        pipelineDesc.HS.pShaderBytecode = nullptr;
        pipelineDesc.GS.BytecodeLength = 0;
        pipelineDesc.GS.pShaderBytecode = nullptr;

        if (description.FragmentModule.IsValid())
        {
            auto d3d12FragmentModule = description.FragmentModule.AsDerived<Graphics::ShaderModuleD3D12>();
            NX_VALIDATE(
                d3d12FragmentModule->GetShaderStage() == Graphics::ShaderStage::Fragment,
                "Shader module is not a fragment shader"
            );
            auto blob = d3d12FragmentModule->GetBlob();

            pipelineDesc.PS.BytecodeLength = blob->GetBufferSize();
            pipelineDesc.PS.pShaderBytecode = blob->GetBufferPointer();
        }

        if (description.GeometryModule.IsValid())
        {
            auto d3d12GeometryModule = description.GeometryModule.AsDerived<Graphics::ShaderModuleD3D12>();
            NX_VALIDATE(
                d3d12GeometryModule->GetShaderStage() == Graphics::ShaderStage::Geometry,
                "Shader module is not a geometry shader"
            );
            auto blob = d3d12GeometryModule->GetBlob();

            pipelineDesc.GS.BytecodeLength = blob->GetBufferSize();
            pipelineDesc.GS.pShaderBytecode = blob->GetBufferPointer();
        }

        if (description.TesselationControlModule.IsValid())
        {
            auto d3d12TesselationControlModule =
                description.TesselationControlModule.AsDerived<Graphics::ShaderModuleD3D12>();
            NX_VALIDATE(
                d3d12TesselationControlModule->GetShaderStage() == Graphics::ShaderStage::TessellationControl,
                "Shader module is not a tesselation control shader"
            );
            auto blob = d3d12TesselationControlModule->GetBlob();

            pipelineDesc.HS.BytecodeLength = blob->GetBufferSize();
            pipelineDesc.HS.pShaderBytecode = blob->GetBufferPointer();
        }

        if (description.TesselationEvaluationModule.IsValid())
        {
            auto d3d12TesselationEvaluationModule =
                description.TesselationEvaluationModule.AsDerived<Graphics::ShaderModuleD3D12>();
            NX_VALIDATE(
                d3d12TesselationEvaluationModule->GetShaderStage() == Graphics::ShaderStage::TessellationEvaluation,
                "Shader module is not a tesselation evaluation shader"
            );
            auto blob = d3d12TesselationEvaluationModule->GetBlob();

            pipelineDesc.DS.BytecodeLength = blob->GetBufferSize();
            pipelineDesc.DS.pShaderBytecode = blob->GetBufferPointer();
        }

        if (description.VertexModule.IsValid())
        {
            auto d3d12VertexModule = description.VertexModule.AsDerived<Graphics::ShaderModuleD3D12>();
            NX_VALIDATE(
                d3d12VertexModule->GetShaderStage() == Graphics::ShaderStage::Vertex,
                "Shader module is not a vertex shader"
            );
            auto blob = d3d12VertexModule->GetBlob();

            pipelineDesc.VS.BytecodeLength = blob->GetBufferSize();
            pipelineDesc.VS.pShaderBytecode = blob->GetBufferPointer();
        }

        pipelineDesc.PrimitiveTopologyType = D3D12::GetPipelineTopology(description.PrimitiveTopology);
        pipelineDesc.RasterizerState = D3D12::CreateRasterizerState(description.RasterizerStateDesc);
        pipelineDesc.StreamOutput = D3D12::CreateStreamOutputDesc();
        pipelineDesc.NumRenderTargets = rtvFormats.size();

        for (uint32_t rtvIndex = 0; rtvIndex < rtvFormats.size(); rtvIndex++)
        {
            pipelineDesc.RTVFormats[rtvIndex] = rtvFormats.at(rtvIndex);
        }

        DXGI_FORMAT depthFormat = D3D12::GetD3D12PixelFormat(description.DepthFormat);
        pipelineDesc.DSVFormat = depthFormat;
        pipelineDesc.BlendState = D3D12::CreateBlendStateDesc(description.ColourBlendStates);
        pipelineDesc.DepthStencilState = D3D12::CreateDepthStencilDesc(description.DepthStencilDesc);
        pipelineDesc.SampleMask = description.SampleMask;
        pipelineDesc.SampleDesc.Count = description.Samples;
        pipelineDesc.SampleDesc.Quality = 0;
        pipelineDesc.NodeMask = 0;
        pipelineDesc.CachedPSO.CachedBlobSizeInBytes = 0;
        pipelineDesc.CachedPSO.pCachedBlob = nullptr;
        pipelineDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

        Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;

        auto d3d12Device = device->GetD3D12Device();
        HRESULT hr = d3d12Device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pso));
        if (FAILED(hr))
        {
            _com_error error(hr);
            std::string message = "Failed to create pipeline state: " + std::string(error.ErrorMessage());
            NX_ERROR(message);
        }
        else
        {
            std::wstring debugName = {description.DebugName.begin(), description.DebugName.end()};
            pso->SetName(debugName.c_str());
        }

        return pso;
    }

    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGraphicsPipelineStream(
        Graphics::GraphicsDeviceD3D12 *device, const Graphics::GraphicsPipelineDescription &description,
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature,
        const std::vector<D3D12_INPUT_ELEMENT_DESC> &inputLayout
    )
    {
        D3D12::StreamStateBuilder builder;
        ID3D12RootSignature *rootSignaturePtr = rootSignature.Get();
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE, rootSignaturePtr);

        if (description.VertexModule.IsValid())
        {
            auto d3d12VertexModule = description.VertexModule.AsDerived<Graphics::ShaderModuleD3D12>();
            NX_VALIDATE(
                d3d12VertexModule->GetShaderStage() == Graphics::ShaderStage::Vertex,
                "Shader module is not a vertex shader"
            );
            auto blob = d3d12VertexModule->GetBlob();

            D3D12_SHADER_BYTECODE byteCode = {};
            byteCode.pShaderBytecode = blob->GetBufferPointer();
            byteCode.BytecodeLength = blob->GetBufferSize();
            builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS, byteCode);
        }

        if (description.FragmentModule.IsValid())
        {
            auto d3d12FragmentModule = description.FragmentModule.AsDerived<Graphics::ShaderModuleD3D12>();
            NX_VALIDATE(
                d3d12FragmentModule->GetShaderStage() == Graphics::ShaderStage::Fragment,
                "Shader module is not a fragment shader"
            );
            auto blob = d3d12FragmentModule->GetBlob();

            D3D12_SHADER_BYTECODE byteCode = {};
            byteCode.pShaderBytecode = blob->GetBufferPointer();
            byteCode.BytecodeLength = blob->GetBufferSize();
            builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS, byteCode);
        }

        if (description.TesselationEvaluationModule.IsValid())
        {
            auto d3d12TessellationEvaluationModule =
                description.TesselationEvaluationModule.AsDerived<Graphics::ShaderModuleD3D12>();
            NX_VALIDATE(
                d3d12TessellationEvaluationModule->GetShaderStage() == Graphics::ShaderStage::TessellationEvaluation,
                "Shader module is not a tessellation evaluation shader"
            );
            auto blob = d3d12TessellationEvaluationModule->GetBlob();

            D3D12_SHADER_BYTECODE byteCode = {};
            byteCode.pShaderBytecode = blob->GetBufferPointer();
            byteCode.BytecodeLength = blob->GetBufferSize();
            builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DS, byteCode);
        }

        if (description.TesselationControlModule.IsValid())
        {
            auto d3d12TessellationControlModule =
                description.TesselationControlModule.AsDerived<Graphics::ShaderModuleD3D12>();
            NX_VALIDATE(
                d3d12TessellationControlModule->GetShaderStage() == Graphics::ShaderStage::TessellationControl,
                "Shader module is not a tessellation control shader"
            );
            auto blob = d3d12TessellationControlModule->GetBlob();

            D3D12_SHADER_BYTECODE byteCode = {};
            byteCode.pShaderBytecode = blob->GetBufferPointer();
            byteCode.BytecodeLength = blob->GetBufferSize();
            builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_HS, byteCode);
        }

        if (description.GeometryModule.IsValid())
        {
            auto d3d12GeometryModule = description.GeometryModule.AsDerived<Graphics::ShaderModuleD3D12>();
            NX_VALIDATE(
                d3d12GeometryModule->GetShaderStage() == Graphics::ShaderStage::Geometry,
                "Shader module is not a geometry shader"
            );
            auto blob = d3d12GeometryModule->GetBlob();

            D3D12_SHADER_BYTECODE byteCode = {};
            byteCode.pShaderBytecode = blob->GetBufferPointer();
            byteCode.BytecodeLength = blob->GetBufferSize();
            builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_GS, byteCode);
        }

        D3D12_BLEND_DESC blendDesc = D3D12::CreateBlendStateDesc(description.ColourBlendStates);
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND, blendDesc);

        D3D12_RASTERIZER_DESC rasterizerDesc = D3D12::CreateRasterizerState(description.RasterizerStateDesc);
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER, rasterizerDesc);

        const auto &deviceFeatures = device->GetPhysicalDeviceFeatures();
        if (deviceFeatures.SupportsDepthBoundsTesting)
        {
            D3D12_DEPTH_STENCIL_DESC1 depthStencilDesc = D3D12::CreateDepthStencilDesc1(description.DepthStencilDesc);
            builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL1, depthStencilDesc);
        }
        else
        {
            D3D12_DEPTH_STENCIL_DESC depthStencilDesc = D3D12::CreateDepthStencilDesc(description.DepthStencilDesc);
            builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL, depthStencilDesc);
        }

        std::vector<D3D12_INPUT_ELEMENT_DESC> layout = inputLayout;
        D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
        inputLayoutDesc.pInputElementDescs = layout.data();
        inputLayoutDesc.NumElements = (UINT)layout.size();
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT, inputLayoutDesc);

        D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopology = D3D12::GetPipelineTopology(description.PrimitiveTopology);
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY, primitiveTopology);

        D3D12_RT_FORMAT_ARRAY rtFormats = {};
        rtFormats.NumRenderTargets = description.ColourTargetCount;
        for (uint32_t index = 0; index < description.ColourTargetCount; index++)
        {
            rtFormats.RTFormats[index] = D3D12::GetD3D12PixelFormat(description.ColourFormats[index]);
        }
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS, rtFormats);

        DXGI_FORMAT depthFormat = D3D12::GetD3D12PixelFormat(description.DepthFormat);
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT, depthFormat);

        DXGI_SAMPLE_DESC sampleDesc = {};
        sampleDesc.Count = description.Samples;
        sampleDesc.Quality = 0;
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC, sampleDesc);

        D3D12_SAMPLE_MASK mask = {};
        mask.SampleMask = description.SampleMask;
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK, mask);

        UINT nodeMask = 0;
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_NODE_MASK, nodeMask);

        D3D12_PIPELINE_STATE_FLAGS flags = D3D12_PIPELINE_STATE_FLAG_NONE;
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_FLAGS, flags);

        Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
        auto d3d12Device = device->GetD3D12Device();

        D3D12_PIPELINE_STATE_STREAM_DESC desc = {};
        desc.pPipelineStateSubobjectStream = builder.GetStream();
        desc.SizeInBytes = builder.GetSizeInBytes();

        HRESULT hr = d3d12Device->CreatePipelineState(&desc, IID_PPV_ARGS(&pso));
        if (FAILED(hr))
        {
            _com_error error(hr);
            std::string message = "Failed to create pipeline state: " + std::string(error.ErrorMessage());
            NX_ERROR(message);
        }
        else
        {
            std::wstring debugName = {description.DebugName.begin(), description.DebugName.end()};
            pso->SetName(debugName.c_str());
        }

        return pso;
    }

    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGraphicsPipeline(
        Graphics::GraphicsDeviceD3D12 *device, const Graphics::GraphicsPipelineDescription &description,
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature,
        const std::vector<D3D12_INPUT_ELEMENT_DESC> &inputLayout
    )
    {
        const Graphics::D3D12DeviceFeatures &features = device->GetD3D12DeviceFeatures();
        if (features.SupportsPipelineStreams)
        {
            return CreateGraphicsPipelineStream(device, description, rootSignature, inputLayout);
        }
        else
        {
            return CreateGraphicsPipelineTraditional(device, description, rootSignature, inputLayout);
        }
    }

    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateComputePipelineStream(
        Graphics::GraphicsDeviceD3D12 *device, const Graphics::ComputePipelineDescription &description,
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature
    )
    {
        auto d3d12ComputeShader = description.ComputeShader.AsDerived<Graphics::ShaderModuleD3D12>();
        NX_VALIDATE(
            d3d12ComputeShader->GetShaderStage() == Graphics::ShaderStage::Compute,
            "Shader provided to ComputePipelineDescription is not a compute shader"
        );

        auto blob = d3d12ComputeShader->GetBlob();

        D3D12::StreamStateBuilder builder;
        ID3D12RootSignature *rootSignaturePtr = rootSignature.Get();
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE, rootSignaturePtr);

        D3D12_SHADER_BYTECODE byteCode = {};
        byteCode.pShaderBytecode = blob->GetBufferPointer();
        byteCode.BytecodeLength = blob->GetBufferSize();
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS, byteCode);

        UINT nodeMask = 0;
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_NODE_MASK, nodeMask);

        D3D12_PIPELINE_STATE_FLAGS flags = D3D12_PIPELINE_STATE_FLAG_NONE;
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_FLAGS, flags);

        Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
        auto d3d12Device = device->GetD3D12Device();

        D3D12_PIPELINE_STATE_STREAM_DESC desc = {};
        desc.pPipelineStateSubobjectStream = builder.GetStream();
        desc.SizeInBytes = builder.GetSizeInBytes();

        HRESULT hr = d3d12Device->CreatePipelineState(&desc, IID_PPV_ARGS(&pso));
        if (FAILED(hr))
        {
            _com_error error(hr);
            std::string message = "Failed to create pipeline state: " + std::string(error.ErrorMessage());
            NX_ERROR(message);
        }

        std::wstring debugName = {description.DebugName.begin(), description.DebugName.end()};
        pso->SetName(debugName.c_str());

        return pso;
    }

    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateComputePipelineTraditional(
        Graphics::GraphicsDeviceD3D12 *device, const Graphics::ComputePipelineDescription &description,
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature
    )
    {
        auto d3d12ComputeShader = description.ComputeShader.AsDerived<Graphics::ShaderModuleD3D12>();
        NX_VALIDATE(
            d3d12ComputeShader->GetShaderStage() == Graphics::ShaderStage::Compute,
            "Shader provided to ComputePipelineDescription is not a compute shader"
        );

        auto blob = d3d12ComputeShader->GetBlob();

        D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
        desc.pRootSignature = rootSignature.Get();
        desc.CS.BytecodeLength = blob->GetBufferSize();
        desc.CS.pShaderBytecode = blob->GetBufferPointer();
        desc.NodeMask = 0;
        desc.CachedPSO.CachedBlobSizeInBytes = 0;
        desc.CachedPSO.pCachedBlob = nullptr;
        desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

        Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;

        auto d3d12Device = device->GetD3D12Device();
        HRESULT hr = d3d12Device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&pso));
        if (FAILED(hr))
        {
            _com_error error(hr);
            std::string message = "Failed to create pipeline state: " + std::string(error.ErrorMessage());
            NX_ERROR(message);
        }
        else
        {
            std::wstring debugName = {description.DebugName.begin(), description.DebugName.end()};
            pso->SetName(debugName.c_str());
        }

        return pso;
    }

    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateComputePipeline(
        Graphics::GraphicsDeviceD3D12 *device, const Graphics::ComputePipelineDescription &description,
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature
    )
    {
        const Graphics::D3D12DeviceFeatures &features = device->GetD3D12DeviceFeatures();
        if (features.SupportsPipelineStreams)
        {
            return CreateComputePipelineStream(device, description, rootSignature);
        }
        else
        {
            return CreateComputePipelineTraditional(device, description, rootSignature);
        }
    }

    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateMeshletPipeline(
        Graphics::GraphicsDeviceD3D12 *device, const Graphics::MeshletPipelineDescription &description,
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature
    )
    {
        D3D12::StreamStateBuilder builder;
        ID3D12RootSignature *rootSignaturePtr = rootSignature.Get();
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE, rootSignaturePtr);

        if (description.TaskModule.IsValid())
        {
            auto d3d12TaskModule = description.TaskModule.AsDerived<Graphics::ShaderModuleD3D12>();
            NX_VALIDATE(
                d3d12TaskModule->GetShaderStage() == Graphics::ShaderStage::Task, "Shader module is not a task shader"
            );
            auto blob = d3d12TaskModule->GetBlob();

            D3D12_SHADER_BYTECODE byteCode = {};
            byteCode.pShaderBytecode = blob->GetBufferPointer();
            byteCode.BytecodeLength = blob->GetBufferSize();
            builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_AS, byteCode);
        }

        if (description.MeshModule.IsValid())
        {
            auto d3d12MeshModule = description.MeshModule.AsDerived<Graphics::ShaderModuleD3D12>();
            NX_VALIDATE(
                d3d12MeshModule->GetShaderStage() == Graphics::ShaderStage::Mesh, "Shader module is not a mesh shader"
            );
            auto blob = d3d12MeshModule->GetBlob();

            D3D12_SHADER_BYTECODE byteCode = {};
            byteCode.pShaderBytecode = blob->GetBufferPointer();
            byteCode.BytecodeLength = blob->GetBufferSize();
            builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MS, byteCode);
        }

        if (description.FragmentModule.IsValid())
        {
            auto d3d12FragmentModule = description.FragmentModule.AsDerived<Graphics::ShaderModuleD3D12>();
            NX_VALIDATE(
                d3d12FragmentModule->GetShaderStage() == Graphics::ShaderStage::Fragment,
                "Shader module is not a fragment shader"
            );
            auto blob = d3d12FragmentModule->GetBlob();

            D3D12_SHADER_BYTECODE byteCode = {};
            byteCode.pShaderBytecode = blob->GetBufferPointer();
            byteCode.BytecodeLength = blob->GetBufferSize();
            builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS, byteCode);
        }

        const auto &deviceFeatures = device->GetPhysicalDeviceFeatures();
        if (deviceFeatures.SupportsDepthBoundsTesting)
        {
            D3D12_DEPTH_STENCIL_DESC1 depthStencilDesc = D3D12::CreateDepthStencilDesc1(description.DepthStencilDesc);
            builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL1, depthStencilDesc);
        }
        else
        {
            D3D12_DEPTH_STENCIL_DESC depthStencilDesc = D3D12::CreateDepthStencilDesc(description.DepthStencilDesc);
            builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL, depthStencilDesc);
        }

        D3D12_BLEND_DESC blendDesc = D3D12::CreateBlendStateDesc(description.ColourBlendStates);
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND, blendDesc);

        D3D12_SAMPLE_MASK mask = {};
        mask.SampleMask = description.SampleMask;
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK, mask);

        D3D12_RASTERIZER_DESC rasterizerDesc = D3D12::CreateRasterizerState(description.RasterizerStateDesc);
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER, rasterizerDesc);

        D3D12_DEPTH_STENCIL_DESC depthStencilDesc = D3D12::CreateDepthStencilDesc(description.DepthStencilDesc);
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL, depthStencilDesc);

        D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopology = D3D12::GetPipelineTopology(description.PrimitiveTopology);
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY, primitiveTopology);

        D3D12_RT_FORMAT_ARRAY rtFormats = {};
        rtFormats.NumRenderTargets = description.ColourTargetCount;
        for (uint32_t index = 0; index < description.ColourTargetCount; index++)
        {
            rtFormats.RTFormats[index] = D3D12::GetD3D12PixelFormat(description.ColourFormats[index]);
        }
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS, rtFormats);

        DXGI_FORMAT depthFormat = D3D12::GetD3D12PixelFormat(description.DepthFormat);
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT, depthFormat);

        DXGI_SAMPLE_DESC sampleDesc = {};
        sampleDesc.Count = description.Samples;
        sampleDesc.Quality = 0;
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC, sampleDesc);

        UINT nodeMask = 0;
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_NODE_MASK, nodeMask);

        D3D12_PIPELINE_STATE_FLAGS flags = D3D12_PIPELINE_STATE_FLAG_NONE;
        builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_FLAGS, flags);

        Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
        auto d3d12Device = device->GetD3D12Device();

        D3D12_PIPELINE_STATE_STREAM_DESC desc = {};
        desc.pPipelineStateSubobjectStream = builder.GetStream();
        desc.SizeInBytes = builder.GetSizeInBytes();

        HRESULT hr = d3d12Device->CreatePipelineState(&desc, IID_PPV_ARGS(&pso));
        if (FAILED(hr))
        {
            _com_error error(hr);
            std::string message = "Failed to create pipeline state: " + std::string(error.ErrorMessage());
            NX_ERROR(message);
        }

        std::wstring debugName = {description.DebugName.begin(), description.DebugName.end()};
        pso->SetName(debugName.c_str());

        return pso;
    }

    D3D12_HEAP_TYPE GetHeapType(const Graphics::DeviceBufferDescription &desc)
    {
        switch (desc.Access)
        {
        case Graphics::BufferMemoryAccess::Upload:
            return D3D12_HEAP_TYPE_UPLOAD;
        case Graphics::BufferMemoryAccess::Default:
            return D3D12_HEAP_TYPE_DEFAULT;
        case Graphics::BufferMemoryAccess::Readback:
            return D3D12_HEAP_TYPE_READBACK;
        default:
            throw std::runtime_error("Failed to find a valid heap type");
        }
    }

    D3D12_RESOURCE_DIMENSION GetResourceDimensions(Nexus::Graphics::TextureType textureType)
    {
        switch (textureType)
        {
        case Nexus::Graphics::TextureType::Texture1D:
            return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
        case Nexus::Graphics::TextureType::Texture2D:
        case Nexus::Graphics::TextureType::TextureCube:
            return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        case Nexus::Graphics::TextureType::Texture3D:
            return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
        default:
            throw std::runtime_error("Failed to find a valid resource dimension");
        }
    }

    D3D12_RESOURCE_FLAGS GetResourceFlags(const Graphics::TextureDescription &description)
    {
        D3D12_RESOURCE_FLAGS flags = {};

        if (description.Usage & Graphics::TextureUsage_ColourAttachment)
        {
            flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        }

        if (description.Usage & Graphics::TextureUsage_DepthStencilAttachment)
        {
            flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        }

        if (description.Usage & Graphics::TextureUsage_Storage)
        {
            flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        }

        return flags;
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC CreateTextureSrvView(const Graphics::TextureViewDescription &desc)
    {
        const Graphics::TextureD3D12 *texture = desc.TargetTexture.AsDerived<const Graphics::TextureD3D12>();

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        switch (texture->GetType())
        {
        case Graphics::TextureType::Texture1D:
            if (desc.Range.LayerCount > 1)
            {
                srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
                srvDesc.Texture1DArray.MostDetailedMip = desc.Range.BaseMipLevel;
                srvDesc.Texture1DArray.MipLevels = desc.Range.LevelCount;
                srvDesc.Texture1DArray.FirstArraySlice = desc.Range.BaseArrayLayer;
                srvDesc.Texture1DArray.ArraySize = desc.Range.LayerCount;
            }
            else
            {
                srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
                srvDesc.Texture1D.MostDetailedMip = desc.Range.BaseMipLevel;
                srvDesc.Texture1D.MipLevels = desc.Range.LevelCount;
            }
            break;

        case Graphics::TextureType::Texture2D:
        {
            if (desc.Range.LayerCount > 1)
            {
                if (texture->GetSampleCount() > 1)
                {
                    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
                    srvDesc.Texture2DMSArray.FirstArraySlice = desc.Range.BaseArrayLayer;
                    srvDesc.Texture2DMSArray.ArraySize = desc.Range.LayerCount;
                }
                else
                {
                    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
                    srvDesc.Texture2DArray.MostDetailedMip = desc.Range.BaseMipLevel;
                    srvDesc.Texture2DArray.MipLevels = desc.Range.LevelCount;
                    srvDesc.Texture2DArray.FirstArraySlice = desc.Range.BaseArrayLayer;
                    srvDesc.Texture2DArray.ArraySize = desc.Range.LayerCount;
                }
            }
            else
            {
                if (texture->GetSampleCount() > 1)
                {
                    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
                }
                else
                {
                    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                    srvDesc.Texture2D.MostDetailedMip = desc.Range.BaseMipLevel;
                    srvDesc.Texture2D.MipLevels = desc.Range.LevelCount;
                }
            }

            break;
        }
        case Graphics::TextureType::Texture3D:
        {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
            srvDesc.Texture3D.MostDetailedMip = desc.Range.BaseMipLevel;
            srvDesc.Texture3D.MipLevels = desc.Range.LevelCount;
            break;
        }
        case Graphics::TextureType::TextureCube:
        {
            if (desc.Range.LayerCount > 6)
            {
                srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
                srvDesc.TextureCubeArray.First2DArrayFace = desc.Range.BaseArrayLayer / 6;
                srvDesc.TextureCubeArray.NumCubes = desc.Range.LayerCount / 6;
                srvDesc.TextureCubeArray.MostDetailedMip = desc.Range.BaseMipLevel;
                srvDesc.TextureCubeArray.MipLevels = desc.Range.LevelCount;
            }
            else
            {
                srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
                srvDesc.TextureCube.MostDetailedMip = desc.Range.BaseMipLevel;
                srvDesc.TextureCube.MipLevels = desc.Range.LevelCount;
            }
            break;
        }
        default:
            throw std::runtime_error("Failed to find a valid TextureType");
        }

        return srvDesc;
    }

    D3D12_UNORDERED_ACCESS_VIEW_DESC CreateTextureUavView(const Graphics::StorageImageView &view)
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC uav = {};

        const Graphics::TextureD3D12 *texture = view.Texture.AsDerived<const Graphics::TextureD3D12>();
        const Graphics::TextureDescription &textureDesc = texture->GetDescription();
        uav.Format = D3D12::GetD3D12PixelFormat(textureDesc.Format);

        switch (textureDesc.Type)
        {
        case Graphics::TextureType::Texture1D:
        {
            uav.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
            uav.Texture1D.MipSlice = view.MipLevel;
            break;
        }
        case Graphics::TextureType::Texture2D:
        {
            {
                uav.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
                uav.Texture2D.MipSlice = view.MipLevel;
                uav.Texture2D.PlaneSlice = 0;
            }
            break;
        }
        case Graphics::TextureType::Texture3D:
        {
            uav.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
            uav.Texture3D.MipSlice = view.MipLevel;
            uav.Texture3D.FirstWSlice = view.ArrayLayer;
            uav.Texture3D.WSize = 1;
            break;
        }
        default:
            throw std::runtime_error("Failed to find a valid TextureType");
        }

        return uav;
    }

    void GetShaderAccessModifiers(Graphics::StorageResourceAccess access, bool &readonly, bool &byteAddress)
    {
        switch (access)
        {
        case Graphics::StorageResourceAccess::Read:
        case Graphics::StorageResourceAccess::ReadStructured:
        {
            readonly = true;
            byteAddress = false;
            return;
        }
        case Graphics::StorageResourceAccess::ReadByteAddress:
        {
            readonly = true;
            byteAddress = true;
            return;
        }
        case Graphics::StorageResourceAccess::Write:
        case Graphics::StorageResourceAccess::ReadWrite:
        case Graphics::StorageResourceAccess::ReadWriteStructured:
        case Graphics::StorageResourceAccess::AppendStructured:
        case Graphics::StorageResourceAccess::ConsumeStructured:
        case Graphics::StorageResourceAccess::ReadWriteStructuredWithCounter:
        {
            readonly = false;
            byteAddress = false;
            return;
        }
        case Graphics::StorageResourceAccess::ReadWriteByteAddress:
        {
            readonly = false;
            byteAddress = true;
            return;
        }

        default:
            throw std::runtime_error("Failed to find a valid access modifier");
        }
    }

    D3D12_SHADER_VISIBILITY GetShaderVisibility(const Graphics::ShaderStageFlags &flags)
    {
        // if we don't specify any shader stages or we supply multiple, then we have
        // to make it visible to all shader stages
        if (flags.IsEmpty() || flags.GetCount() > 1)
        {
            return D3D12_SHADER_VISIBILITY_ALL;
        }

        // otherwise return the correct shader stage
        if (flags.HasFlag(Graphics::ShaderStage::Vertex))
        {
            return D3D12_SHADER_VISIBILITY_VERTEX;
        }
        else if (flags.HasFlag(Graphics::ShaderStage::TessellationControl))
        {
            return D3D12_SHADER_VISIBILITY_HULL;
        }
        else if (flags.HasFlag(Graphics::ShaderStage::TessellationEvaluation))
        {
            return D3D12_SHADER_VISIBILITY_DOMAIN;
        }
        else if (flags.HasFlag(Graphics::ShaderStage::Geometry))
        {
            return D3D12_SHADER_VISIBILITY_GEOMETRY;
        }
        else if (flags.HasFlag(Graphics::ShaderStage::Fragment))
        {
            return D3D12_SHADER_VISIBILITY_PIXEL;
        }
        else if (flags.HasFlag(Graphics::ShaderStage::Task))
        {
            return D3D12_SHADER_VISIBILITY_AMPLIFICATION;
        }
        else if (flags.HasFlag(Graphics::ShaderStage::Mesh))
        {
            return D3D12_SHADER_VISIBILITY_MESH;
        }
        else
        {
            return D3D12_SHADER_VISIBILITY_ALL;
        }
    }

    D3D12_DESCRIPTOR_RANGE_TYPE GetDescriptorRangeType(const Graphics::ShaderResource &resource)
    {
        switch (resource.Type)
        {
        case Graphics::ResourceType::AccelerationStructure:
        case Graphics::ResourceType::Texture:
        {
            return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        }
        case Graphics::ResourceType::Sampler:
        case Graphics::ResourceType::ComparisonSampler:
        {
            return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
        }
        case Graphics::ResourceType::UniformBuffer:
        {
            return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
        }
        case Graphics::ResourceType::StorageImage:
        case Graphics::ResourceType::StorageTextureBuffer:
        case Graphics::ResourceType::CombinedImageSampler:
        case Graphics::ResourceType::StorageBuffer:
        case Graphics::ResourceType::UniformTextureBuffer:
        {
            switch (resource.Access)
            {
            case Graphics::StorageResourceAccess::Read:
            case Graphics::StorageResourceAccess::ReadByteAddress:
            case Graphics::StorageResourceAccess::ReadStructured:
            {
                return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            }
            case Graphics::StorageResourceAccess::Write:
            case Graphics::StorageResourceAccess::ReadWrite:
            case Graphics::StorageResourceAccess::ReadWriteStructured:
            case Graphics::StorageResourceAccess::ReadWriteByteAddress:
            case Graphics::StorageResourceAccess::AppendStructured:
            case Graphics::StorageResourceAccess::ConsumeStructured:
            case Graphics::StorageResourceAccess::ReadWriteStructuredWithCounter:
            case Graphics::StorageResourceAccess::FeedbackTexture:
            {
                return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
            }
            default:
                throw std::runtime_error("Failed to find a valid access type");
            }
        }
        default:
            throw std::runtime_error("Failed to find a valid descriptor range type");
        }
    }

    // a util struct to temporarily store the different types of ranges prior to
    // creating the root signature
    struct DescriptorRangeInfo
    {
        std::vector<D3D12_DESCRIPTOR_RANGE> SamplerRanges = {};
        std::vector<D3D12_DESCRIPTOR_RANGE> OtherRanges = {};
        std::map<std::string, D3D12_ROOT_CONSTANTS> RootConstants = {};
        std::map<std::string, D3D12_ROOT_DESCRIPTOR> RootCBVs = {};
        std::map<std::string, D3D12_ROOT_DESCRIPTOR> RootSRVs = {};
        std::map<std::string, D3D12_ROOT_DESCRIPTOR> RootUAVs = {};
    };

    static std::optional<Graphics::ResourceDescriptor> GetDescriptorFromResourceSetDesc(
        const std::string &name, const Graphics::ResourceSetDescription &resourceSetDesc
    )
    {
        for (const Graphics::ResourceDescriptor &descriptor : resourceSetDesc.Descriptors)
        {
            if (descriptor.Name == name)
            {
                return descriptor;
            }
        }

        return {};
    }

    static void CreateDescriptorRangeMap(
        const std::map<std::string, Graphics::ShaderResource> &reflectedResources,
        const Graphics::ResourceSetDescription &requestedResources,
        std::map<D3D12_SHADER_VISIBILITY, DescriptorRangeInfo> &descriptorRangeMap,
        std::map<std::string, uint32_t> &rootParameterIndexes, DescriptorHandleInfo &descriptorHandleInfo
    )
    {
        uint32_t samplerIndex = 0;
        uint32_t nonSamplerIndex = 0;
        uint32_t rootParameterIndex = 0;

        for (const auto &[name, resourceInfo] : reflectedResources)
        {
            D3D12_SHADER_VISIBILITY visibility = GetShaderVisibility(resourceInfo.Stage);
            D3D12_DESCRIPTOR_RANGE_TYPE descriptorType = GetDescriptorRangeType(resourceInfo);

            // samplers cannot share a descriptor range with other descriptors so we
            // need them to be separate
            if (descriptorType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER)
            {
                D3D12_DESCRIPTOR_RANGE &range = descriptorRangeMap[visibility].SamplerRanges.emplace_back();
                range.RangeType = GetDescriptorRangeType(resourceInfo);
                range.BaseShaderRegister = resourceInfo.Binding;
                range.NumDescriptors = resourceInfo.ResourceCount;
                range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
                range.RegisterSpace = resourceInfo.RegisterSpace;

                // retrieve and then increment offset
                for (size_t i = 0; i < resourceInfo.ResourceCount; i++)
                {
                    descriptorHandleInfo.SamplerIndexes[resourceInfo.Name].push_back(samplerIndex + i);
                }

                samplerIndex += resourceInfo.ResourceCount;
                descriptorHandleInfo.SamplerHeapCount += resourceInfo.ResourceCount;
            }
            else
            {
                RootParameterType rootParameterType = RootParameterType::CBV_SRV_UAV_HeapRange;
                size_t countOf32BitValues = 0;

                std::optional<Graphics::ResourceDescriptor> requestedDescriptor =
                    GetDescriptorFromResourceSetDesc(name, requestedResources);

                // try to find the correct descriptor type for the resource, if it
                // exists in the provided ResourceSetDescription
                if (requestedDescriptor.has_value())
                {
                    Graphics::ResourceDescriptor descriptor = requestedDescriptor.value();

                    if (descriptor.Type == Graphics::ResourceDescriptorType::PushConstants ||
                        descriptor.Type == Graphics::ResourceDescriptorType::InlineUniformBlock)
                    {
                        rootParameterType = RootParameterType::RootConstants;
                        countOf32BitValues = descriptor.CountOrSizeInBytes / 4;
                    }
                    else if (descriptor.Type == Graphics::ResourceDescriptorType::DynamicUniformBuffer)
                    {
                        rootParameterType = RootParameterType::RootCBV;
                    }
                    else if (descriptor.Type == Graphics::ResourceDescriptorType::DynamicStorageBuffer)
                    {
                        Graphics::StorageResourceAccess access = resourceInfo.Access;
                        bool readonly = {};
                        bool byteAddress = {};
                        D3D12::GetShaderAccessModifiers(access, readonly, byteAddress);

                        if (readonly)
                        {
                            rootParameterType = RootParameterType::RootSRV;
                        }
                        else
                        {
                            rootParameterType = RootParameterType::RootUAV;
                        }
                    }
                }

                // we are creating a range in a sampler descriptor table
                if (rootParameterType == RootParameterType::SamplerHeapRange)
                {
                    D3D12_DESCRIPTOR_RANGE &range = descriptorRangeMap[visibility].SamplerRanges.emplace_back();
                    range.RangeType = GetDescriptorRangeType(resourceInfo);
                    range.BaseShaderRegister = resourceInfo.Binding;
                    range.NumDescriptors = resourceInfo.ResourceCount;
                    range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
                    range.RegisterSpace = resourceInfo.RegisterSpace;

                    for (size_t i = 0; i < resourceInfo.ResourceCount; i++)
                    {
                        descriptorHandleInfo.SamplerIndexes[resourceInfo.Name].push_back(samplerIndex + i);
                    }

                    // retrieve and then increment offset
                    samplerIndex += resourceInfo.ResourceCount;
                    descriptorHandleInfo.SamplerHeapCount += resourceInfo.ResourceCount;
                }
                // creating range in sampler heap
                else if (rootParameterType == RootParameterType::CBV_SRV_UAV_HeapRange)
                {
                    D3D12_DESCRIPTOR_RANGE &range = descriptorRangeMap[visibility].OtherRanges.emplace_back();

                    range.RangeType = GetDescriptorRangeType(resourceInfo);
                    range.BaseShaderRegister = resourceInfo.Binding;
                    range.NumDescriptors = resourceInfo.ResourceCount;
                    range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
                    range.RegisterSpace = resourceInfo.RegisterSpace;

                    for (size_t i = 0; i < resourceInfo.ResourceCount; i++)
                    {
                        descriptorHandleInfo.NonSamplerIndexes[resourceInfo.Name].push_back(nonSamplerIndex + i);
                    }

                    // retrieve and then increment offset
                    nonSamplerIndex += resourceInfo.ResourceCount;
                    descriptorHandleInfo.SRV_UAV_CBV_HeapCount += resourceInfo.ResourceCount;

                    // if the resource is a storage buffer, we need to record how to
                    // bind it correctly in D3D12, e.g.
                    // StructuredBuffer/ByteAddressBuffer
                    if (resourceInfo.Type == Graphics::ResourceType::StorageBuffer)
                    {
                        descriptorHandleInfo.StorageBuffers[name] = resourceInfo.Access;
                    }
                }
                // creating root constants
                else if (rootParameterType == RootParameterType::RootConstants)
                {
                    D3D12_ROOT_CONSTANTS &constants = descriptorRangeMap[visibility].RootConstants[name];
                    constants.Num32BitValues = countOf32BitValues;
                    constants.RegisterSpace = resourceInfo.RegisterSpace;
                    constants.ShaderRegister = resourceInfo.Binding;

                    rootParameterIndexes[name] = rootParameterIndex++;
                }
                // creating root CBV
                else if (rootParameterType == RootParameterType::RootCBV)
                {
                    for (uint32_t shaderRegister = resourceInfo.Binding;
                         shaderRegister < resourceInfo.Binding + resourceInfo.ResourceCount; shaderRegister++)
                    {
                        D3D12_ROOT_DESCRIPTOR &descriptor = descriptorRangeMap[visibility].RootCBVs[name];
                        descriptor.RegisterSpace = resourceInfo.RegisterSpace;
                        descriptor.ShaderRegister = shaderRegister;

                        rootParameterIndexes[name] = rootParameterIndex++;
                    }
                }
                // creating root SRV
                else if (rootParameterType == RootParameterType::RootSRV)
                {
                    for (uint32_t shaderRegister = resourceInfo.Binding;
                         shaderRegister < resourceInfo.Binding + resourceInfo.ResourceCount; shaderRegister++)
                    {
                        D3D12_ROOT_DESCRIPTOR &descriptor = descriptorRangeMap[visibility].RootSRVs[name];
                        descriptor.RegisterSpace = resourceInfo.RegisterSpace;
                        descriptor.ShaderRegister = shaderRegister;

                        rootParameterIndexes[name] = rootParameterIndex++;
                    }
                }
                // creating root UAV
                else if (rootParameterType == RootParameterType::RootUAV)
                {
                    for (uint32_t shaderRegister = resourceInfo.Binding;
                         shaderRegister < resourceInfo.Binding + resourceInfo.ResourceCount; shaderRegister++)
                    {
                        D3D12_ROOT_DESCRIPTOR &descriptor = descriptorRangeMap[visibility].RootUAVs[name];
                        descriptor.RegisterSpace = resourceInfo.RegisterSpace;
                        descriptor.ShaderRegister = shaderRegister;

                        rootParameterIndexes[name] = rootParameterIndex++;
                    }
                }
                else
                {
                    throw std::runtime_error("Failed to find a valid descriptor type");
                }
            }
        }
    }

    static void FindCombinedImageSamplers(
        const std::map<std::string, Graphics::ShaderResource> &resources, DescriptorHandleInfo &descriptorHandleInfo
    )
    {
        // loop through all resources to find textures
        for (const auto &[textureName, textureInfo] : resources)
        {
            // if the resource is a texture, loop through to find any samplers
            if (textureInfo.Type == Graphics::ResourceType::Texture)
            {
                for (const auto &[samplerName, samplerInfo] : resources)
                {
                    // if the resource is a sampler, then we need to compare it
                    // against the texture to check if it forms a combined image
                    // sampler
                    if (samplerInfo.Type == Graphics::ResourceType::Sampler)
                    {
                        // we have found a combined image sampler
                        if (textureInfo.Binding == samplerInfo.Binding &&
                            textureInfo.ResourceCount == samplerInfo.ResourceCount)
                        {
                            descriptorHandleInfo.CombinedImageSamplerMap[textureName] = samplerName;
                        }
                    }
                }
            }
        }
    }

    static void CreateDescriptorRanges(
        const std::map<D3D12_SHADER_VISIBILITY, DescriptorRangeInfo> &descriptorMap,
        DescriptorHandleInfo &descriptorHandleInfo, std::vector<D3D12_ROOT_PARAMETER> &rootParameters,
        std::map<std::string, uint32_t> &rootParameterIndexes,
        RootSignatureBindingLocations &rootSignatureBindingLocation
    )
    {
        size_t currentSamplerOffset = 0;
        size_t currentNonSamplerOffset = 0;

        uint32_t rootParameterIndex = 0;

        // create the descriptor tables for the ranges
        for (const auto &[shaderVisibility, descriptorRange] : descriptorMap)
        {
            // create samplers, if needed
            if (!descriptorRange.SamplerRanges.empty())
            {
                D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable = {};
                descriptorTable.pDescriptorRanges = descriptorRange.SamplerRanges.data();
                descriptorTable.NumDescriptorRanges = descriptorRange.SamplerRanges.size();

                D3D12_ROOT_PARAMETER &rootParameter = rootParameters.emplace_back();
                rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                rootParameter.DescriptorTable = descriptorTable;
                rootParameter.ShaderVisibility = shaderVisibility;

                DescriptorTableInfo &descriptorTableInfo = descriptorHandleInfo.DescriptorTables.emplace_back();
                descriptorTableInfo.Source = DescriptorHandleSource::ISampler;
                descriptorTableInfo.Offset = currentSamplerOffset;

                RootParameterBindingLocation &samplerBindingLocation =
                    rootSignatureBindingLocation.HeapBindings.emplace_back();
                samplerBindingLocation.ParameterType = RootParameterType::SamplerHeapRange;
                samplerBindingLocation.RootParameterIndex = rootParameterIndex++;
                samplerBindingLocation.DescriptorOffset = currentSamplerOffset;

                for (const auto &range : descriptorRange.SamplerRanges)
                {
                    currentSamplerOffset += range.NumDescriptors;
                }
            }

            // create ranges for all referenced resources, if needed
            if (!descriptorRange.OtherRanges.empty())
            {
                D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable = {};
                descriptorTable.pDescriptorRanges = descriptorRange.OtherRanges.data();
                descriptorTable.NumDescriptorRanges = descriptorRange.OtherRanges.size();

                D3D12_ROOT_PARAMETER &rootParameter = rootParameters.emplace_back();
                rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                rootParameter.DescriptorTable = descriptorTable;
                rootParameter.ShaderVisibility = shaderVisibility;

                DescriptorTableInfo &descriptorTableInfo = descriptorHandleInfo.DescriptorTables.emplace_back();
                descriptorTableInfo.Source = DescriptorHandleSource::SRV_UAV_CBV;
                descriptorTableInfo.Offset = currentNonSamplerOffset;

                RootParameterBindingLocation &samplerBindingLocation =
                    rootSignatureBindingLocation.HeapBindings.emplace_back();
                samplerBindingLocation.ParameterType = RootParameterType::CBV_SRV_UAV_HeapRange;
                samplerBindingLocation.RootParameterIndex = rootParameterIndex++;
                samplerBindingLocation.DescriptorOffset = currentNonSamplerOffset;

                for (const auto &range : descriptorRange.OtherRanges)
                {
                    currentNonSamplerOffset += range.NumDescriptors;
                }
            }

            for (const auto &[name, rootConstants] : descriptorRange.RootConstants)
            {
                D3D12_ROOT_PARAMETER &rootParameter = rootParameters.emplace_back();
                rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
                rootParameter.Constants = rootConstants;
                rootParameter.ShaderVisibility = shaderVisibility;

                RootParameterBindingLocation &samplerBindingLocation =
                    rootSignatureBindingLocation.DynamicResources[name];
                samplerBindingLocation.ParameterType = RootParameterType::RootConstants;
                samplerBindingLocation.RootParameterIndex = rootParameterIndex++;
                samplerBindingLocation.DescriptorOffset = 0;
            }

            for (const auto &[name, rootCBV] : descriptorRange.RootCBVs)
            {
                D3D12_ROOT_PARAMETER &rootParameter = rootParameters.emplace_back();
                rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
                rootParameter.Descriptor = rootCBV;
                rootParameter.ShaderVisibility = shaderVisibility;

                RootParameterBindingLocation &samplerBindingLocation =
                    rootSignatureBindingLocation.DynamicResources[name];
                samplerBindingLocation.ParameterType = RootParameterType::RootCBV;
                samplerBindingLocation.RootParameterIndex = rootParameterIndex++;
                samplerBindingLocation.DescriptorOffset = 0;
            }

            for (const auto &[name, rootSRV] : descriptorRange.RootSRVs)
            {
                D3D12_ROOT_PARAMETER &rootParameter = rootParameters.emplace_back();
                rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
                rootParameter.Descriptor = rootSRV;
                rootParameter.ShaderVisibility = shaderVisibility;

                RootParameterBindingLocation &samplerBindingLocation =
                    rootSignatureBindingLocation.DynamicResources[name];
                samplerBindingLocation.ParameterType = RootParameterType::RootSRV;
                samplerBindingLocation.RootParameterIndex = rootParameterIndex++;
                samplerBindingLocation.DescriptorOffset = 0;
            }

            for (const auto &[name, rootUAV] : descriptorRange.RootUAVs)
            {
                D3D12_ROOT_PARAMETER &rootParameter = rootParameters.emplace_back();
                rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
                rootParameter.Descriptor = rootUAV;
                rootParameter.ShaderVisibility = shaderVisibility;

                RootParameterBindingLocation &samplerBindingLocation =
                    rootSignatureBindingLocation.DynamicResources[name];
                samplerBindingLocation.ParameterType = RootParameterType::RootUAV;
                samplerBindingLocation.RootParameterIndex = rootParameterIndex++;
                samplerBindingLocation.DescriptorOffset = 0;
            }
        }
    }

    void CreateRootSignature(
        const std::map<std::string, Graphics::ShaderResource> &reflectedResources,
        const Graphics::ResourceSetDescription &requestedResources, Microsoft::WRL::ComPtr<ID3D12Device9> device,
        Microsoft::WRL::ComPtr<ID3DBlob> &inRootSignatureBlob,
        Microsoft::WRL::ComPtr<ID3D12RootSignature> &inRootSignature, DescriptorHandleInfo &descriptorHandleInfo,
        RootSignatureBindingLocations &rootSignatureBindingLocation, bool requiresInputAssembly
    )
    {
        // create storage for descriptor ranges and root parameters
        std::map<D3D12_SHADER_VISIBILITY, DescriptorRangeInfo> descriptorRanges = {};
        std::vector<D3D12_ROOT_PARAMETER> rootParameters = {};
        std::map<std::string, uint32_t> rootParameterIndexes = {};

        CreateDescriptorRangeMap(
            reflectedResources, requestedResources, descriptorRanges, rootParameterIndexes, descriptorHandleInfo
        );
        FindCombinedImageSamplers(reflectedResources, descriptorHandleInfo);
        CreateDescriptorRanges(
            descriptorRanges, descriptorHandleInfo, rootParameters, rootParameterIndexes, rootSignatureBindingLocation
        );

        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

        if (requiresInputAssembly)
        {
            rootSignatureFlags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        }

        if (!descriptorRanges.contains(D3D12_SHADER_VISIBILITY_VERTEX))
        {
            rootSignatureFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
        }

        if (!descriptorRanges.contains(D3D12_SHADER_VISIBILITY_HULL))
        {
            rootSignatureFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
        }

        if (!descriptorRanges.contains(D3D12_SHADER_VISIBILITY_DOMAIN))
        {
            rootSignatureFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
        }

        if (!descriptorRanges.contains(D3D12_SHADER_VISIBILITY_GEOMETRY))
        {
            rootSignatureFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
        }

        if (!descriptorRanges.contains(D3D12_SHADER_VISIBILITY_PIXEL))
        {
            rootSignatureFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
        }

        if (!descriptorRanges.contains(D3D12_SHADER_VISIBILITY_AMPLIFICATION))
        {
            rootSignatureFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS;
        }

        if (!descriptorRanges.contains(D3D12_SHADER_VISIBILITY_MESH))
        {
            rootSignatureFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS;
        }

        std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers = {};
        for (const auto &[name, samplers] : requestedResources.ImmutableSamplers)
        {
            const auto &resourceInfo = reflectedResources.at(name);

            for (size_t samplerIndex = 0; samplerIndex < samplers.size(); samplerIndex++)
            {
                const auto &sampler = samplers[samplerIndex];
                const Graphics::SamplerDescription &samplerDesc = sampler->GetSamplerDescription();

                D3D12_STATIC_SAMPLER_DESC &staticSampler = staticSamplers.emplace_back();
                staticSampler.Filter = D3D12::GetD3D12Filter(samplerDesc.SampleFilter);
                staticSampler.AddressU = D3D12::GetD3D12TextureAddressMode(samplerDesc.AddressModeU);
                staticSampler.AddressV = D3D12::GetD3D12TextureAddressMode(samplerDesc.AddressModeV);
                staticSampler.AddressW = D3D12::GetD3D12TextureAddressMode(samplerDesc.AddressModeW);
                staticSampler.MipLODBias = samplerDesc.LODBias;
                staticSampler.MaxAnisotropy = samplerDesc.MaximumAnisotropy;
                staticSampler.ComparisonFunc = D3D12::GetComparisonFunction(samplerDesc.SamplerComparisonFunction);

                switch (samplerDesc.TextureBorderColor)
                {
                case Graphics::BorderColour::TransparentBlack:
                    staticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
                    break;
                case Graphics::BorderColour::OpaqueBlack:
                    staticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
                    break;
                case Graphics::BorderColour::OpaqueWhite:
                    staticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
                    break;
                default:
                    throw std::runtime_error("Failed to find a valid BorderColor");
                }

                staticSampler.MinLOD = samplerDesc.MinimumLOD;
                staticSampler.MaxLOD = samplerDesc.MaximumLOD;
                staticSampler.ShaderRegister = resourceInfo.Binding + static_cast<UINT>(samplerIndex);
                staticSampler.RegisterSpace = resourceInfo.RegisterSpace;
                staticSampler.ShaderVisibility = GetShaderVisibility(resourceInfo.Stage);
            }
        }

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
        rootSignatureDesc.Flags = rootSignatureFlags;
        rootSignatureDesc.NumStaticSamplers = staticSamplers.size();
        rootSignatureDesc.pStaticSamplers = staticSamplers.data();
        rootSignatureDesc.NumParameters = rootParameters.size();
        rootSignatureDesc.pParameters = rootParameters.data();

        // serialize the root signature and report any errors if they occur
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
        if (SUCCEEDED(D3D12SerializeRootSignature(
                &rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &inRootSignatureBlob, &errorBlob
            )))
        {
            device->CreateRootSignature(
                0, inRootSignatureBlob->GetBufferPointer(), inRootSignatureBlob->GetBufferSize(),
                IID_PPV_ARGS(&inRootSignature)
            );
        }
        else
        {
            std::string errorMessage = std::string((char *)errorBlob->GetBufferPointer());
            throw std::runtime_error(errorMessage);
        }
    }

    std::vector<D3D12_INPUT_ELEMENT_DESC> CreateInputLayout(const std::vector<Graphics::VertexBufferLayout> &layouts)
    {
        std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayouts = {};

        uint32_t elementIndex = 0;
        for (uint32_t layoutIndex = 0; layoutIndex < layouts.size(); layoutIndex++)
        {
            const auto &layout = layouts.at(layoutIndex);

            for (uint32_t i = 0; i < layout.GetNumberOfElements(); i++)
            {
                const auto &element = layout.GetElement(i);

                D3D12_INPUT_CLASSIFICATION classification = layout.IsInstanceBuffer()
                                                                ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA
                                                                : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;

                D3D12_INPUT_ELEMENT_DESC desc = {
                    element.Name.c_str(),
                    elementIndex,
                    D3D12::GetD3D12BaseType(element),
                    layoutIndex,
                    (UINT)element.Offset,
                    classification,
                    0
                };

                if (layout.IsInstanceBuffer())
                {
                    desc.InstanceDataStepRate = layout.GetInstanceStepRate();
                }

                inputLayouts.push_back(desc);
                elementIndex++;
            }
        }

        return inputLayouts;
    }

    D3D_PRIMITIVE_TOPOLOGY CreatePrimitiveTopology(Graphics::Topology topology)
    {
        switch (topology)
        {
        case Graphics::Topology::LineList:
            return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
        case Graphics::Topology::LineStrip:
            return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
        case Graphics::Topology::PointList:
            return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
        case Graphics::Topology::TriangleList:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        case Graphics::Topology::TriangleStrip:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        default:
            throw std::runtime_error("Failed to find a valid topology");
        }
    }

    D3D12_RASTERIZER_DESC CreateRasterizerState(const Graphics::RasterizerStateDescription &rasterizerState)
    {
        D3D12_RASTERIZER_DESC desc{};
        desc.FillMode = D3D12_FILL_MODE_SOLID;
        desc.CullMode = D3D12::GetCullMode(rasterizerState.TriangleCullMode);

        if (rasterizerState.TriangleFrontFace == Nexus::Graphics::FrontFace::CounterClockwise)
        {
            desc.FrontCounterClockwise = true;
        }
        else
        {
            desc.FrontCounterClockwise = false;
        }

        desc.DepthBias = rasterizerState.DepthBias;
        desc.DepthBiasClamp = rasterizerState.DepthBiasClamp;
        desc.SlopeScaledDepthBias = rasterizerState.SlopeScaledDepthBias;
        desc.DepthClipEnable = rasterizerState.DepthClipEnabled ? TRUE : FALSE;
        desc.MultisampleEnable = FALSE;
        desc.AntialiasedLineEnable = FALSE;
        desc.ForcedSampleCount = 0;
        return desc;
    }

    D3D12_STREAM_OUTPUT_DESC CreateStreamOutputDesc()
    {
        D3D12_STREAM_OUTPUT_DESC desc{};
        desc.NumEntries = 0;
        desc.NumStrides = 0;
        desc.pBufferStrides = nullptr;
        desc.RasterizedStream = 0;
        return desc;
    }

    D3D12_BLEND_DESC CreateBlendStateDesc(const std::array<Graphics::BlendStateDescription, 8> &colourBlendStates)
    {
        D3D12_BLEND_DESC desc{};
        desc.AlphaToCoverageEnable = FALSE;
        desc.IndependentBlendEnable = TRUE;

        for (size_t i = 0; i < colourBlendStates.size(); i++)
        {
            desc.RenderTarget[i].BlendEnable = colourBlendStates[i].EnableBlending;
            desc.RenderTarget[i].SrcBlend = D3D12::GetBlendFunction(colourBlendStates[i].SourceColourBlend);
            desc.RenderTarget[i].SrcBlend = D3D12::GetBlendFunction(colourBlendStates[i].SourceColourBlend);
            desc.RenderTarget[i].DestBlend = D3D12::GetBlendFunction(colourBlendStates[i].DestinationColourBlend);
            desc.RenderTarget[i].BlendOp = D3D12::GetBlendEquation(colourBlendStates[i].ColorBlendFunction);
            desc.RenderTarget[i].SrcBlendAlpha = D3D12::GetBlendFunction(colourBlendStates[i].SourceAlphaBlend);
            desc.RenderTarget[i].DestBlendAlpha = D3D12::GetBlendFunction(colourBlendStates[i].DestinationAlphaBlend);
            desc.RenderTarget[i].BlendOpAlpha = D3D12::GetBlendEquation(colourBlendStates[i].AlphaBlendFunction);
            desc.RenderTarget[i].LogicOpEnable = FALSE;
            desc.RenderTarget[i].LogicOp = D3D12_LOGIC_OP_NOOP;

            uint8_t writeMask = 0;
            if (colourBlendStates[i].PixelWriteMask.Red)
            {
                writeMask |= D3D12_COLOR_WRITE_ENABLE_RED;
            }
            if (colourBlendStates[i].PixelWriteMask.Green)
            {
                writeMask |= D3D12_COLOR_WRITE_ENABLE_GREEN;
            }
            if (colourBlendStates[i].PixelWriteMask.Blue)
            {
                writeMask |= D3D12_COLOR_WRITE_ENABLE_BLUE;
            }
            if (colourBlendStates[i].PixelWriteMask.Alpha)
            {
                writeMask |= D3D12_COLOR_WRITE_ENABLE_ALPHA;
            }

            desc.RenderTarget[i].RenderTargetWriteMask = writeMask;
        }

        return desc;
    }

    D3D12_DEPTH_STENCIL_DESC CreateDepthStencilDesc(const Graphics::DepthStencilDescription &depthStencilDesc)
    {
        D3D12_DEPTH_STENCIL_DESC desc{};
        desc.DepthEnable = depthStencilDesc.EnableDepthTest;
        desc.DepthFunc = D3D12::GetComparisonFunction(depthStencilDesc.DepthComparisonFunction);

        if (depthStencilDesc.EnableDepthWrite)
        {
            desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        }
        else
        {
            desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        }

        desc.StencilEnable = depthStencilDesc.EnableStencilTest;
        desc.StencilReadMask = depthStencilDesc.StencilCompareMask;
        desc.StencilWriteMask = depthStencilDesc.StencilWriteMask;

        desc.FrontFace.StencilFunc = D3D12::GetComparisonFunction(depthStencilDesc.Front.StencilComparisonFunction);
        desc.FrontFace.StencilDepthFailOp =
            D3D12::GetStencilOperation(depthStencilDesc.Front.StencilSuccessDepthFailOperation);
        desc.FrontFace.StencilFailOp = D3D12::GetStencilOperation(depthStencilDesc.Front.StencilFailOperation);
        desc.FrontFace.StencilPassOp =
            D3D12::GetStencilOperation(depthStencilDesc.Front.StencilSuccessDepthSuccessOperation);

        desc.BackFace.StencilFunc = D3D12::GetComparisonFunction(depthStencilDesc.Back.StencilComparisonFunction);
        desc.BackFace.StencilDepthFailOp =
            D3D12::GetStencilOperation(depthStencilDesc.Back.StencilSuccessDepthFailOperation);
        desc.BackFace.StencilFailOp = D3D12::GetStencilOperation(depthStencilDesc.Back.StencilFailOperation);
        desc.BackFace.StencilPassOp =
            D3D12::GetStencilOperation(depthStencilDesc.Back.StencilSuccessDepthSuccessOperation);

        return desc;
    }

    D3D12_DEPTH_STENCIL_DESC1 CreateDepthStencilDesc1(const Graphics::DepthStencilDescription &depthStencilDesc)
    {
        D3D12_DEPTH_STENCIL_DESC1 desc = {};
        desc.DepthEnable = depthStencilDesc.EnableDepthTest;
        desc.DepthFunc = D3D12::GetComparisonFunction(depthStencilDesc.DepthComparisonFunction);

        if (depthStencilDesc.EnableDepthWrite)
        {
            desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        }
        else
        {
            desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        }

        desc.StencilEnable = depthStencilDesc.EnableStencilTest;
        desc.StencilReadMask = depthStencilDesc.StencilCompareMask;
        desc.StencilWriteMask = depthStencilDesc.StencilWriteMask;

        desc.FrontFace.StencilFunc = D3D12::GetComparisonFunction(depthStencilDesc.Front.StencilComparisonFunction);
        desc.FrontFace.StencilDepthFailOp =
            D3D12::GetStencilOperation(depthStencilDesc.Front.StencilSuccessDepthFailOperation);
        desc.FrontFace.StencilFailOp = D3D12::GetStencilOperation(depthStencilDesc.Front.StencilFailOperation);
        desc.FrontFace.StencilPassOp =
            D3D12::GetStencilOperation(depthStencilDesc.Front.StencilSuccessDepthSuccessOperation);

        desc.BackFace.StencilFunc = D3D12::GetComparisonFunction(depthStencilDesc.Back.StencilComparisonFunction);
        desc.BackFace.StencilDepthFailOp =
            D3D12::GetStencilOperation(depthStencilDesc.Back.StencilSuccessDepthFailOperation);
        desc.BackFace.StencilFailOp = D3D12::GetStencilOperation(depthStencilDesc.Back.StencilFailOperation);
        desc.BackFace.StencilPassOp =
            D3D12::GetStencilOperation(depthStencilDesc.Back.StencilSuccessDepthSuccessOperation);

        desc.DepthBoundsTestEnable = depthStencilDesc.EnableDepthsBoundsTest;

        return desc;
    }

    UINT GetSyncIntervalFromPresentMode(Graphics::PresentMode presentMode)
    {
        switch (presentMode)
        {
        case Graphics::PresentMode::Immediate:
        {
            return 0;
        }
        case Graphics::PresentMode::Mailbox:
        case Graphics::PresentMode::Fifo:
        case Graphics::PresentMode::FifoRelaxed:
        {
            return 1;
        }
        default:
            throw std::runtime_error("Failed to find a valid present mode");
        }

        return 0;
    }

    D3D12_RESOURCE_STATES GetTextureResourceState(Graphics::TextureLayout layout)
    {
        switch (layout)
        {
        case Graphics::TextureLayout::Undefined:
            return D3D12_RESOURCE_STATE_COMMON;
        case Graphics::TextureLayout::General:
            return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        case Graphics::TextureLayout::ColourAttachmentOptimal:
            return D3D12_RESOURCE_STATE_RENDER_TARGET;
        case Graphics::TextureLayout::DepthStencilAttachmentOptimal:
            return D3D12_RESOURCE_STATE_DEPTH_WRITE;
        case Graphics::TextureLayout::DepthStencilReadOnlyOptimal:
            return D3D12_RESOURCE_STATE_DEPTH_READ;
        case Graphics::TextureLayout::ShaderReadOnlyOptimal:
            return D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
        case Graphics::TextureLayout::TransferSrcOptimal:
            return D3D12_RESOURCE_STATE_COPY_SOURCE;
        case Graphics::TextureLayout::TransferDstOptimal:
            return D3D12_RESOURCE_STATE_COPY_DEST;
        case Graphics::TextureLayout::ResolveSrc:
            return D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
        case Graphics::TextureLayout::ResolveDest:
            return D3D12_RESOURCE_STATE_RESOLVE_DEST;
        case Graphics::TextureLayout::PresentSrc:
            return D3D12_RESOURCE_STATE_PRESENT;
        case Graphics::TextureLayout::VideoEncodeDestination:
            return D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE;
        case Graphics::TextureLayout::VideoEncodeSource:
            return D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ;
        case Graphics::TextureLayout::VideoDecodeDestination:
            return D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE;
        case Graphics::TextureLayout::VideoDecodeSource:
            return D3D12_RESOURCE_STATE_VIDEO_DECODE_READ;
        default:
            throw std::runtime_error("Failed to find a valid resource state");
        }
    }

    D3D12_BARRIER_SYNC GetBarrierSync(Graphics::BarrierPipelineStage stage)
    {
        switch (stage)
        {
        case Graphics::BarrierPipelineStage::NoStage:
            return D3D12_BARRIER_SYNC_NONE;
        case Graphics::BarrierPipelineStage::DrawIndirect:
            return D3D12_BARRIER_SYNC_EXECUTE_INDIRECT;
        case Graphics::BarrierPipelineStage::VertexInput:
            return D3D12_BARRIER_SYNC_INDEX_INPUT;
        case Graphics::BarrierPipelineStage::VertexShader:
            return D3D12_BARRIER_SYNC_VERTEX_SHADING;
        case Graphics::BarrierPipelineStage::TessellationControlShader:
            return D3D12_BARRIER_SYNC_ALL_SHADING;
        case Graphics::BarrierPipelineStage::TessellationEvaluationShader:
            return D3D12_BARRIER_SYNC_ALL_SHADING;
        case Graphics::BarrierPipelineStage::GeometryShader:
            return D3D12_BARRIER_SYNC_ALL_SHADING;
        case Graphics::BarrierPipelineStage::FragmentShader:
            return D3D12_BARRIER_SYNC_PIXEL_SHADING;
        case Graphics::BarrierPipelineStage::EarlyFragmentTests:
        case Graphics::BarrierPipelineStage::LateFragmentTests:
            return D3D12_BARRIER_SYNC_DEPTH_STENCIL;
        case Graphics::BarrierPipelineStage::ColourAttachmentOutput:
            return D3D12_BARRIER_SYNC_RENDER_TARGET;
        case Graphics::BarrierPipelineStage::ComputeShader:
            return D3D12_BARRIER_SYNC_COMPUTE_SHADING;
        case Graphics::BarrierPipelineStage::AllTransfers:
        case Graphics::BarrierPipelineStage::Transfer:
        case Graphics::BarrierPipelineStage::Host:
            return D3D12_BARRIER_SYNC_COPY;
        case Graphics::BarrierPipelineStage::AllGraphics:
            return D3D12_BARRIER_SYNC_ALL_SHADING;
        case Graphics::BarrierPipelineStage::AllCommands:
            return D3D12_BARRIER_SYNC_ALL;
        case Graphics::BarrierPipelineStage::Copy:
            return D3D12_BARRIER_SYNC_COPY;
        case Graphics::BarrierPipelineStage::Resolve:
            return D3D12_BARRIER_SYNC_RESOLVE;
        case Graphics::BarrierPipelineStage::IndexInput:
            return D3D12_BARRIER_SYNC_INDEX_INPUT;
        case Graphics::BarrierPipelineStage::VertexAttributeInput:
            return D3D12_BARRIER_SYNC_VERTEX_SHADING;
        case Graphics::BarrierPipelineStage::PreRasterizationShaders:
            return D3D12_BARRIER_SYNC_ALL_SHADING;
        case Graphics::BarrierPipelineStage::TransformFeedback:
            return D3D12_BARRIER_SYNC_ALL_SHADING;
        case Graphics::BarrierPipelineStage::AccelerationStructure:
        case Graphics::BarrierPipelineStage::RayTracingShader:
            return D3D12_BARRIER_SYNC_RAYTRACING;
        case Graphics::BarrierPipelineStage::TaskShader:
        case Graphics::BarrierPipelineStage::MeshShader:
            return D3D12_BARRIER_SYNC_ALL_SHADING;

        default:
            throw std::runtime_error("Failed to find a valid pipeline stage");
        }
    }

    D3D12_BARRIER_ACCESS GetBarrierAccess(Graphics::BarrierAccess access)
    {
        switch (access)
        {
        case Graphics::BarrierAccess::NoAccess:
            return D3D12_BARRIER_ACCESS_NO_ACCESS;
        case Graphics::BarrierAccess::IndirectCommandRead:
            return D3D12_BARRIER_ACCESS_INDIRECT_ARGUMENT;
        case Graphics::BarrierAccess::IndexRead:
            return D3D12_BARRIER_ACCESS_INDEX_BUFFER;
        case Graphics::BarrierAccess::VertexAttributeRead:
            return D3D12_BARRIER_ACCESS_VERTEX_BUFFER;
        case Graphics::BarrierAccess::UniformRead:
            return D3D12_BARRIER_ACCESS_CONSTANT_BUFFER;
        case Graphics::BarrierAccess::InputAttachmentRead:
            return D3D12_BARRIER_ACCESS_RENDER_TARGET;
        case Graphics::BarrierAccess::ShaderRead:
            return D3D12_BARRIER_ACCESS_SHADER_RESOURCE;
        case Graphics::BarrierAccess::ShaderWrite:
            return D3D12_BARRIER_ACCESS_UNORDERED_ACCESS;
        case Graphics::BarrierAccess::ColourAttachmentRead:
        case Graphics::BarrierAccess::ColourAttachmentWrite:
            return D3D12_BARRIER_ACCESS_RENDER_TARGET;
        case Graphics::BarrierAccess::DepthStencilAttachmentRead:
            return D3D12_BARRIER_ACCESS_DEPTH_STENCIL_READ;
        case Graphics::BarrierAccess::DepthStencilAttachmentWrite:
            return D3D12_BARRIER_ACCESS_DEPTH_STENCIL_WRITE;
        case Graphics::BarrierAccess::TransferRead:
            return D3D12_BARRIER_ACCESS_COPY_SOURCE;
        case Graphics::BarrierAccess::TransferWrite:
            return D3D12_BARRIER_ACCESS_COPY_DEST;
        case Graphics::BarrierAccess::HostRead:
            return D3D12_BARRIER_ACCESS_COPY_SOURCE;
        case Graphics::BarrierAccess::HostWrite:
            return D3D12_BARRIER_ACCESS_COPY_DEST;
        case Graphics::BarrierAccess::MemoryRead:
            return D3D12_BARRIER_ACCESS_COPY_SOURCE;
        case Graphics::BarrierAccess::MemoryWrite:
            return D3D12_BARRIER_ACCESS_COPY_DEST;
        case Graphics::BarrierAccess::TransformFeedbackWrite:
            return D3D12_BARRIER_ACCESS_STREAM_OUTPUT;
        case Graphics::BarrierAccess::AccelerationStructureRead:
            return D3D12_BARRIER_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_READ;
        case Graphics::BarrierAccess::AccelerationStructureWrite:
            return D3D12_BARRIER_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_WRITE;
        case Graphics::BarrierAccess::VideoDecode:
            return D3D12_BARRIER_ACCESS_VIDEO_DECODE_READ | D3D12_BARRIER_ACCESS_VIDEO_DECODE_WRITE;
        case Graphics::BarrierAccess::VideoEncode:
            return D3D12_BARRIER_ACCESS_VIDEO_ENCODE_READ | D3D12_BARRIER_ACCESS_VIDEO_ENCODE_WRITE;
        default:
            throw std::runtime_error("Failed to find a valid access type");
        }
    }

    D3D12_BARRIER_LAYOUT GetBarrierLayout(Graphics::TextureLayout layout)
    {
        switch (layout)
        {
        case Graphics::TextureLayout::Undefined:
            return D3D12_BARRIER_LAYOUT_UNDEFINED;
        case Graphics::TextureLayout::General:
            return D3D12_BARRIER_LAYOUT_COMMON;
        case Graphics::TextureLayout::ColourAttachmentOptimal:
            return D3D12_BARRIER_LAYOUT_RENDER_TARGET;
        case Graphics::TextureLayout::DepthStencilAttachmentOptimal:
            return D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_WRITE;
        case Graphics::TextureLayout::DepthStencilReadOnlyOptimal:
            return D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_READ;
        case Graphics::TextureLayout::ShaderReadOnlyOptimal:
            return D3D12_BARRIER_LAYOUT_SHADER_RESOURCE;
        case Graphics::TextureLayout::TransferSrcOptimal:
            return D3D12_BARRIER_LAYOUT_COPY_SOURCE;
        case Graphics::TextureLayout::TransferDstOptimal:
            return D3D12_BARRIER_LAYOUT_COPY_DEST;
        case Graphics::TextureLayout::ResolveSrc:
            return D3D12_BARRIER_LAYOUT_RESOLVE_SOURCE;
        case Graphics::TextureLayout::ResolveDest:
            return D3D12_BARRIER_LAYOUT_RESOLVE_DEST;
        case Graphics::TextureLayout::PresentSrc:
            return D3D12_BARRIER_LAYOUT_PRESENT;
        case Graphics::TextureLayout::VideoEncodeDestination:
            return D3D12_BARRIER_LAYOUT_VIDEO_ENCODE_WRITE;
        case Graphics::TextureLayout::VideoEncodeSource:
            return D3D12_BARRIER_LAYOUT_VIDEO_ENCODE_READ;
        default:
            throw std::runtime_error("Failed to find a valid barrier layout");
        }
    }

    D3D12_RESOURCE_FLAGS GetResourceFlags(uint8_t usage)
    {
        D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

        if (usage & Graphics::BufferUsage_Storage)
        {
            flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        }

        if (usage & Graphics::BufferUsage_AccelerationStructureBuildInputReadOnly ||
            usage & Graphics::BufferUsage_AccelerationStructureStorage)
        {
            flags |= D3D12_RESOURCE_FLAG_RAYTRACING_ACCELERATION_STRUCTURE;
        }

        return flags;
    }

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE GetAccelerationStructureType(Graphics::AccelerationStructureType type)
    {
        switch (type)
        {
        case Graphics::AccelerationStructureType::BottomLevel:
            return D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
        case Graphics::AccelerationStructureType::TopLevel:
            return D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
        default:
            throw std::runtime_error("Failed to find a valid acceleration structure type");
        }
    }

    static D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS GetAccelerationStructureBuildFlags(
        uint8_t flags, bool performUpdate
    )
    {
        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags =
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;

        if (flags & Graphics::AccelerationStructureBuildFlags::AllowUpdate)
        {
            buildFlags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
        }

        if (flags & Graphics::AccelerationStructureBuildFlags::AllowCompaction)
        {
            buildFlags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_COMPACTION;
        }

        if (flags & Graphics::AccelerationStructureBuildFlags::PreferFastTrace)
        {
            buildFlags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
        }

        if (flags & Graphics::AccelerationStructureBuildFlags::PreferFastBuild)
        {
            buildFlags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_BUILD;
        }
        if (flags & Graphics::AccelerationStructureBuildFlags::MinimizeMemory)
        {
            buildFlags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_MINIMIZE_MEMORY;
        }

        if (performUpdate)
        {
            buildFlags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;
        }

        return buildFlags;
    }

    static D3D12_RAYTRACING_GEOMETRY_TYPE GetRayTracingGeometryType(Graphics::GeometryType type)
    {
        switch (type)
        {
        case Graphics::GeometryType::AxisAlignedBoundingBoxes:
            return D3D12_RAYTRACING_GEOMETRY_TYPE_PROCEDURAL_PRIMITIVE_AABBS;
        case Graphics::GeometryType::Triangles:
            return D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
        default:
            throw std::runtime_error("Failed to find a valid geometry type");
        }
    }

    static D3D12_RAYTRACING_GEOMETRY_FLAGS GetRayTracingGeometryFlags(uint8_t flags)
    {
        D3D12_RAYTRACING_GEOMETRY_FLAGS outputFlags = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;

        if (flags & Graphics::AccelerationStructureGeometryFlags::Opaque)
        {
            outputFlags |= D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
        }

        if (flags & Graphics::AccelerationStructureGeometryFlags::NoDuplicateAnyhit)
        {
            outputFlags |= D3D12_RAYTRACING_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT_INVOCATION;
        }

        return outputFlags;
    }

    static DXGI_FORMAT GetVertexFormat(Graphics::VertexFormat format)
    {
        switch (format)
        {
        case Graphics::VertexFormat::R32G32_SFloat:
            return DXGI_FORMAT_R32G32_FLOAT;
        case Graphics::VertexFormat::R32G32B32_SFloat:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case Graphics::VertexFormat::R16G16_SFloat:
            return DXGI_FORMAT_R16G16_FLOAT;
        case Graphics::VertexFormat::R16G16B16A16_SFloat:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case Graphics::VertexFormat::R16G16_SNorm:
            return DXGI_FORMAT_R16G16_SNORM;
        case Graphics::VertexFormat::R16G16B16A16_SNorm:
            return DXGI_FORMAT_R16G16B16A16_SNORM;
        case Graphics::VertexFormat::R16G16B16A16_UNorm:
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        case Graphics::VertexFormat::R16G16_UNorm:
            return DXGI_FORMAT_R16G16_UNORM;
        case Graphics::VertexFormat::R10G10B10A2_UNorm:
            return DXGI_FORMAT_R10G10B10A2_UNORM;
        case Graphics::VertexFormat::R8G8_UNorm:
            return DXGI_FORMAT_R8G8_UNORM;
        case Graphics::VertexFormat::R8G8B8A8_UNorm:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case Graphics::VertexFormat::R8G8_SNorm:
            return DXGI_FORMAT_R8G8_SNORM;
        default:
            throw std::runtime_error("Failed to find a valid vertex format");
        }
    }

    static void GetAccelerationStructureBuildGeometry(
        const Graphics::AccelerationStructureGeometryBuildDescription &description,
        std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> &outputGeometry, bool &isInstance,
        D3D12_GPU_VIRTUAL_ADDRESS &instanceAddress, uint32_t &instanceCount
    )
    {
        isInstance = false;

        for (const auto &buildGeometry : description.Geometry)
        {
            if (buildGeometry.Type != Graphics::GeometryType::Instance)
            {
            }

            switch (buildGeometry.Type)
            {
            case Graphics::GeometryType::AxisAlignedBoundingBoxes:
            {
                Graphics::AccelerationStructureAABBGeometry aabbs =
                    std::get<Graphics::AccelerationStructureAABBGeometry>(buildGeometry.Geometry);

                D3D12_GPU_VIRTUAL_ADDRESS_AND_STRIDE address = {
                    .StartAddress = aabbs.AABBs, .StrideInBytes = aabbs.Stride
                };

                D3D12_RAYTRACING_GEOMETRY_DESC &geometryDesc = outputGeometry.emplace_back();
                geometryDesc.Type = GetRayTracingGeometryType(buildGeometry.Type);
                geometryDesc.Flags = GetRayTracingGeometryFlags(buildGeometry.Flags);
                geometryDesc.AABBs.AABBs = address;
                geometryDesc.AABBs.AABBCount = aabbs.Count;
                break;
            }
            case Graphics::GeometryType::Triangles:
            {
                Graphics::AccelerationStructureTriangleGeometry triangles =
                    std::get<Graphics::AccelerationStructureTriangleGeometry>(buildGeometry.Geometry);

                D3D12_GPU_VIRTUAL_ADDRESS_AND_STRIDE vertexDataAddress = {
                    .StartAddress = triangles.VertexBuffer, .StrideInBytes = triangles.VertexBufferStride
                };
                D3D12_GPU_VIRTUAL_ADDRESS indexDataAddress = triangles.IndexBuffer;
                D3D12_GPU_VIRTUAL_ADDRESS transformDataAddress = triangles.TransformBuffer;

                D3D12_RAYTRACING_GEOMETRY_DESC &geometryDesc = outputGeometry.emplace_back();
                geometryDesc.Type = GetRayTracingGeometryType(buildGeometry.Type);
                geometryDesc.Flags = GetRayTracingGeometryFlags(buildGeometry.Flags);
                geometryDesc.Triangles.VertexFormat = GetVertexFormat(triangles.VertexBufferFormat);
                geometryDesc.Triangles.VertexBuffer = vertexDataAddress;
                geometryDesc.Triangles.VertexCount = triangles.VertexCount;

                if (triangles.IndexBufferFormat.has_value())
                {
                    size_t indexSize = Graphics::GetIndexFormatSizeInBytes(triangles.IndexBufferFormat.value());

                    geometryDesc.Triangles.IndexBuffer = 0;
                    geometryDesc.Triangles.IndexFormat = GetD3D12IndexBufferFormat(triangles.IndexBufferFormat.value());
                    geometryDesc.Triangles.IndexCount = triangles.IndexCount;
                }
                else
                {
                    geometryDesc.Triangles.IndexBuffer = indexDataAddress;
                    geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_UNKNOWN;
                    geometryDesc.Triangles.IndexCount = 0;
                }

                break;
            }

            case Graphics::GeometryType::Instance:
            {
                Graphics::AccelerationStructureInstanceGeometry instances =
                    std::get<Graphics::AccelerationStructureInstanceGeometry>(buildGeometry.Geometry);

                isInstance = true;
                instanceCount = instances.Count;
                break;
            }
            default:
                throw std::runtime_error("Failed to find a valid geometry type");
            }
        }
    }

    void GetD3D12AccelerationStructureInputs(
        const Graphics::AccelerationStructureGeometryBuildDescription &description,
        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS &inputs,
        std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> &geometry
    )
    {
        bool performUpdate = description.Mode == Graphics::AccelerationStructureBuildMode::Update;

        bool isInstance = false;
        std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> geometryDesc = {};
        D3D12_GPU_VIRTUAL_ADDRESS instanceAddress = {};
        uint32_t instanceCount = 0;
        GetAccelerationStructureBuildGeometry(description, geometryDesc, isInstance, instanceAddress, instanceCount);

        inputs.Type = GetAccelerationStructureType(description.Type);
        inputs.Flags = GetAccelerationStructureBuildFlags(description.Flags, performUpdate);
        inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;

        if (isInstance)
        {
            inputs.NumDescs = instanceCount;
            inputs.InstanceDescs = instanceAddress;
        }
        else
        {
            inputs.NumDescs = geometryDesc.size();
            inputs.pGeometryDescs = geometryDesc.data();
        }
    }

    void GetD3D12FeatureLevelAsMajorMinor(D3D_FEATURE_LEVEL level, uint32_t &major, uint32_t &minor)
    {
        switch (level)
        {
        case D3D_FEATURE_LEVEL_1_0_GENERIC:
        case D3D_FEATURE_LEVEL_1_0_CORE:
        {
            major = 1;
            minor = 0;
            return;
        }
        case D3D_FEATURE_LEVEL_9_1:
        {
            major = 9;
            minor = 1;
            return;
        }
        case D3D_FEATURE_LEVEL_9_2:
        {
            major = 9;
            minor = 2;
            return;
        }
        case D3D_FEATURE_LEVEL_9_3:
        {
            major = 9;
            minor = 3;
            return;
        }
        case D3D_FEATURE_LEVEL_10_0:
        {
            major = 10;
            minor = 0;
            return;
        }
        case D3D_FEATURE_LEVEL_10_1:
        {
            major = 10;
            minor = 1;
            return;
        }
        case D3D_FEATURE_LEVEL_11_0:
        {
            major = 11;
            minor = 0;
            return;
        }
        case D3D_FEATURE_LEVEL_11_1:
        {
            major = 11;
            minor = 1;
            return;
        }
        case D3D_FEATURE_LEVEL_12_0:
        {
            major = 12;
            minor = 0;
            return;
        }
        case D3D_FEATURE_LEVEL_12_1:
        {
            major = 12;
            minor = 1;
            return;
        }
        case D3D_FEATURE_LEVEL_12_2:
        {
            major = 12;
            minor = 2;
            return;
        }

        default:
            throw std::runtime_error("Failed to find a valid feature level");
        }
    }
} // namespace Nexus::D3D12
#endif