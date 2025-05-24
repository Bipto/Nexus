#include "D3D12Utils.hpp"

#if defined(NX_PLATFORM_D3D12)

	#include "Nexus-Core/nxpch.hpp"

namespace Nexus::D3D12
{
	DXGI_FORMAT GetD3D12PixelFormat(Nexus::Graphics::PixelFormat format, bool isDepth)
	{
		switch (format)
		{
			case Nexus::Graphics::PixelFormat::R8_UNorm: return DXGI_FORMAT_R8_UNORM;
			case Nexus::Graphics::PixelFormat::R8_SNorm: return DXGI_FORMAT_R8_SNORM;
			case Nexus::Graphics::PixelFormat::R8_UInt: return DXGI_FORMAT_R8_UINT;
			case Nexus::Graphics::PixelFormat::R8_SInt: return DXGI_FORMAT_R8_SINT;

			case Nexus::Graphics::PixelFormat::R16_UNorm: return isDepth ? DXGI_FORMAT_R16_TYPELESS : DXGI_FORMAT_R16_UNORM;
			case Nexus::Graphics::PixelFormat::R16_SNorm: return DXGI_FORMAT_R16_SNORM;
			case Nexus::Graphics::PixelFormat::R16_UInt: return DXGI_FORMAT_R16_UINT;
			case Nexus::Graphics::PixelFormat::R16_SInt: return DXGI_FORMAT_R16_SINT;
			case Nexus::Graphics::PixelFormat::R16_Float: return DXGI_FORMAT_R16_FLOAT;

			case Nexus::Graphics::PixelFormat::R32_UInt: return DXGI_FORMAT_R32_UINT;
			case Nexus::Graphics::PixelFormat::R32_SInt: return DXGI_FORMAT_R32_SINT;
			case Nexus::Graphics::PixelFormat::R32_Float: return isDepth ? DXGI_FORMAT_R32_TYPELESS : DXGI_FORMAT_R32_FLOAT;

			case Nexus::Graphics::PixelFormat::R8_G8_UNorm: return DXGI_FORMAT_R8G8_UNORM;
			case Nexus::Graphics::PixelFormat::R8_G8_SNorm: return DXGI_FORMAT_R8G8_SNORM;
			case Nexus::Graphics::PixelFormat::R8_G8_UInt: return DXGI_FORMAT_R8G8_UINT;
			case Nexus::Graphics::PixelFormat::R8_G8_SInt: return DXGI_FORMAT_R8G8_SINT;

			case Nexus::Graphics::PixelFormat::R16_G16_UNorm: return DXGI_FORMAT_R16G16_UNORM;
			case Nexus::Graphics::PixelFormat::R16_G16_SNorm: return DXGI_FORMAT_R16G16_SNORM;
			case Nexus::Graphics::PixelFormat::R16_G16_UInt: return DXGI_FORMAT_R16G16_UINT;
			case Nexus::Graphics::PixelFormat::R16_G16_SInt: return DXGI_FORMAT_R16G16_SINT;
			case Nexus::Graphics::PixelFormat::R16_G16_Float: return DXGI_FORMAT_R16G16_FLOAT;

			case Nexus::Graphics::PixelFormat::R32_G32_UInt: return DXGI_FORMAT_R32G32_UINT;
			case Nexus::Graphics::PixelFormat::R32_G32_SInt: return DXGI_FORMAT_R32G32_SINT;
			case Nexus::Graphics::PixelFormat::R32_G32_Float: return DXGI_FORMAT_R32G32_FLOAT;

			case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
			case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			case Nexus::Graphics::PixelFormat::B8_G8_R8_A8_UNorm: return DXGI_FORMAT_B8G8R8A8_UNORM;
			case Nexus::Graphics::PixelFormat::B8_G8_R8_A8_UNorm_SRGB: return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
			case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_SNorm: return DXGI_FORMAT_R8G8B8A8_SNORM;
			case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UInt: return DXGI_FORMAT_R8G8B8A8_UINT;
			case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_SInt: return DXGI_FORMAT_R8G8B8A8_SINT;

			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_UNorm: return DXGI_FORMAT_R16G16B16A16_UNORM;
			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_SNorm: return DXGI_FORMAT_R16G16B16A16_SNORM;
			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_UInt: return DXGI_FORMAT_R16G16B16A16_UINT;
			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_SInt: return DXGI_FORMAT_R16G16B16A16_SINT;
			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_Float: return DXGI_FORMAT_R16G16B16A16_FLOAT;

			case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_UInt: return DXGI_FORMAT_R32G32B32A32_UINT;
			case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_SInt: return DXGI_FORMAT_R32G32B32A32_SINT;
			case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_Float: return DXGI_FORMAT_R32G32B32A32_FLOAT;

			case Nexus::Graphics::PixelFormat::D24_UNorm_S8_UInt: assert(isDepth); return DXGI_FORMAT_D24_UNORM_S8_UINT;
			case Nexus::Graphics::PixelFormat::D32_Float_S8_UInt: assert(isDepth); return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

			case Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UNorm: return DXGI_FORMAT_R10G10B10A2_UNORM;
			case Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UInt: return DXGI_FORMAT_R10G10B10A2_UINT;
			case Nexus::Graphics::PixelFormat::R11_G11_B10_Float: return DXGI_FORMAT_R11G11B10_FLOAT;
			default: throw std::runtime_error("Failed to find a valid format");
		}
	}

	D3D12_CULL_MODE GetCullMode(Nexus::Graphics::CullMode cullMode)
	{
		switch (cullMode)
		{
			case Nexus::Graphics::CullMode::Back: return D3D12_CULL_MODE_BACK;
			case Nexus::Graphics::CullMode::Front: return D3D12_CULL_MODE_FRONT;
			case Nexus::Graphics::CullMode::CullNone: return D3D12_CULL_MODE_NONE;
			default: throw std::runtime_error("Failed to find a valid format");
		}
	}

	D3D12_COMPARISON_FUNC
	GetComparisonFunction(Nexus::Graphics::ComparisonFunction function)
	{
		switch (function)
		{
			case Nexus::Graphics::ComparisonFunction::AlwaysPass: return D3D12_COMPARISON_FUNC_ALWAYS;
			case Nexus::Graphics::ComparisonFunction::Equal: return D3D12_COMPARISON_FUNC_EQUAL;
			case Nexus::Graphics::ComparisonFunction::Greater: return D3D12_COMPARISON_FUNC_GREATER;
			case Nexus::Graphics::ComparisonFunction::GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
			case Nexus::Graphics::ComparisonFunction::Less: return D3D12_COMPARISON_FUNC_LESS;
			case Nexus::Graphics::ComparisonFunction::LessEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
			case Nexus::Graphics::ComparisonFunction::Never: return D3D12_COMPARISON_FUNC_NEVER;
			case Nexus::Graphics::ComparisonFunction::NotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
			default: throw std::runtime_error("Failed to find a valid format");
		}
	}

	D3D12_STENCIL_OP
	GetStencilOperation(Nexus::Graphics::StencilOperation operation)
	{
		switch (operation)
		{
			case Nexus::Graphics::StencilOperation::Keep: return D3D12_STENCIL_OP_KEEP;
			case Nexus::Graphics::StencilOperation::Zero: return D3D12_STENCIL_OP_ZERO;
			case Nexus::Graphics::StencilOperation::Replace: return D3D12_STENCIL_OP_REPLACE;
			case Nexus::Graphics::StencilOperation::Increment: return D3D12_STENCIL_OP_INCR;
			case Nexus::Graphics::StencilOperation::Decrement: return D3D12_STENCIL_OP_DECR;
			case Nexus::Graphics::StencilOperation::Invert: return D3D12_STENCIL_OP_INVERT;
			default: NX_ASSERT(0, "Invalid stencil operation entered"); return D3D12_STENCIL_OP();
		}
	}

	D3D12_FILL_MODE GetFillMode(Nexus::Graphics::FillMode fillMode)
	{
		switch (fillMode)
		{
			case Nexus::Graphics::FillMode::Solid: return D3D12_FILL_MODE_SOLID;
			case Nexus::Graphics::FillMode::Wireframe: return D3D12_FILL_MODE_WIREFRAME;
			default: NX_ASSERT(0, "Invalid fill mode entered"); return D3D12_FILL_MODE();
		}
	}

	D3D12_BLEND GetBlendFunction(Nexus::Graphics::BlendFactor function)
	{
		switch (function)
		{
			case Nexus::Graphics::BlendFactor::Zero: return D3D12_BLEND_ZERO;
			case Nexus::Graphics::BlendFactor::One: return D3D12_BLEND_ONE;
			case Nexus::Graphics::BlendFactor::SourceColor: return D3D12_BLEND_SRC_COLOR;
			case Nexus::Graphics::BlendFactor::OneMinusSourceColor: return D3D12_BLEND_INV_SRC_COLOR;
			case Nexus::Graphics::BlendFactor::DestinationColor: return D3D12_BLEND_DEST_COLOR;
			case Nexus::Graphics::BlendFactor::OneMinusDestinationColor: return D3D12_BLEND_INV_DEST_COLOR;
			case Nexus::Graphics::BlendFactor::SourceAlpha: return D3D12_BLEND_SRC_ALPHA;
			case Nexus::Graphics::BlendFactor::OneMinusSourceAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
			case Nexus::Graphics::BlendFactor::DestinationAlpha: return D3D12_BLEND_DEST_ALPHA;
			case Nexus::Graphics::BlendFactor::OneMinusDestinationAlpha: return D3D12_BLEND_INV_DEST_ALPHA;
			default: throw std::runtime_error("Failed to find a valid blend function");
		}
	}

	D3D12_BLEND_OP GetBlendEquation(Nexus::Graphics::BlendEquation equation)
	{
		switch (equation)
		{
			case Nexus::Graphics::BlendEquation::Add: return D3D12_BLEND_OP_ADD;
			case Nexus::Graphics::BlendEquation::Subtract: return D3D12_BLEND_OP_SUBTRACT;
			case Nexus::Graphics::BlendEquation::ReverseSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
			case Nexus::Graphics::BlendEquation::Min: return D3D12_BLEND_OP_MIN;
			case Nexus::Graphics::BlendEquation::Max: return D3D12_BLEND_OP_MAX;
			default: throw std::runtime_error("Failed to find a valid blend operation");
		}
	}

	DXGI_FORMAT
	GetD3D12BaseType(const Nexus::Graphics::VertexBufferElement &element)
	{
		switch (element.Type)
		{
			case Nexus::Graphics::ShaderDataType::Byte: return DXGI_FORMAT_R8_UINT;
			case Nexus::Graphics::ShaderDataType::Byte2: return DXGI_FORMAT_R8G8_UINT;
			case Nexus::Graphics::ShaderDataType::Byte4: return DXGI_FORMAT_R8G8B8A8_UINT;

			case Nexus::Graphics::ShaderDataType::NormByte: return DXGI_FORMAT_R8_UNORM;
			case Nexus::Graphics::ShaderDataType::NormByte2: return DXGI_FORMAT_R8G8_UNORM;
			case Nexus::Graphics::ShaderDataType::NormByte4: return DXGI_FORMAT_R8G8B8A8_UNORM;

			case Nexus::Graphics::ShaderDataType::Float: return DXGI_FORMAT_R32_FLOAT;
			case Nexus::Graphics::ShaderDataType::Float2: return DXGI_FORMAT_R32G32_FLOAT;
			case Nexus::Graphics::ShaderDataType::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
			case Nexus::Graphics::ShaderDataType::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;

			case Nexus::Graphics::ShaderDataType::Half: return DXGI_FORMAT_R16_FLOAT;
			case Nexus::Graphics::ShaderDataType::Half2: return DXGI_FORMAT_R16G16_FLOAT;
			case Nexus::Graphics::ShaderDataType::Half4: return DXGI_FORMAT_R16G16B16A16_FLOAT;

			case Nexus::Graphics::ShaderDataType::Int: return DXGI_FORMAT_R32_SINT;
			case Nexus::Graphics::ShaderDataType::Int2: return DXGI_FORMAT_R32G32_SINT;
			case Nexus::Graphics::ShaderDataType::Int3: return DXGI_FORMAT_R32G32B32_SINT;
			case Nexus::Graphics::ShaderDataType::Int4: return DXGI_FORMAT_R32G32B32A32_SINT;

			case Nexus::Graphics::ShaderDataType::SignedByte: return DXGI_FORMAT_R8_SNORM;
			case Nexus::Graphics::ShaderDataType::SignedByte2: return DXGI_FORMAT_R8G8_SNORM;
			case Nexus::Graphics::ShaderDataType::SignedByte4: return DXGI_FORMAT_R8G8B8A8_SNORM;

			case Nexus::Graphics::ShaderDataType::SignedByteNormalized: return DXGI_FORMAT_R8_SNORM;
			case Nexus::Graphics::ShaderDataType::SignedByte2Normalized: return DXGI_FORMAT_R8G8_SNORM;
			case Nexus::Graphics::ShaderDataType::SignedByte4Normalized: return DXGI_FORMAT_R8G8B8A8_SNORM;

			case Nexus::Graphics::ShaderDataType::Short: return DXGI_FORMAT_R16_UINT;
			case Nexus::Graphics::ShaderDataType::Short2: return DXGI_FORMAT_R16G16_UINT;
			case Nexus::Graphics::ShaderDataType::Short4: return DXGI_FORMAT_R16G16B16A16_UINT;

			case Nexus::Graphics::ShaderDataType::ShortNormalized: return DXGI_FORMAT_R16_SNORM;
			case Nexus::Graphics::ShaderDataType::Short2Normalized: return DXGI_FORMAT_R16G16_SNORM;
			case Nexus::Graphics::ShaderDataType::Short4Normalized: return DXGI_FORMAT_R16G16B16A16_SNORM;

			case Nexus::Graphics::ShaderDataType::UInt: return DXGI_FORMAT_R32_UINT;
			case Nexus::Graphics::ShaderDataType::UInt2: return DXGI_FORMAT_R32G32_UINT;
			case Nexus::Graphics::ShaderDataType::UInt3: return DXGI_FORMAT_R32G32B32_UINT;
			case Nexus::Graphics::ShaderDataType::UInt4: return DXGI_FORMAT_R32G32B32A32_UINT;

			case Nexus::Graphics::ShaderDataType::UShort: return DXGI_FORMAT_R16_UINT;
			case Nexus::Graphics::ShaderDataType::UShort2: return DXGI_FORMAT_R16G16_UINT;
			case Nexus::Graphics::ShaderDataType::UShort4: return DXGI_FORMAT_R16G16B16A16_UINT;

			case Nexus::Graphics::ShaderDataType::UShortNormalized: return DXGI_FORMAT_R16_UNORM;
			case Nexus::Graphics::ShaderDataType::UShort2Normalized: return DXGI_FORMAT_R16G16_UNORM;
			case Nexus::Graphics::ShaderDataType::UShort4Normalized: return DXGI_FORMAT_R16G16B16A16_UNORM;
			default: throw std::runtime_error("Failed to find valid vertex buffer element type");
		}
	}

	D3D12_FILTER GetD3D12Filter(Nexus::Graphics::SamplerFilter filter)
	{
		switch (filter)
		{
			case Nexus::Graphics::SamplerFilter::Anisotropic: return D3D12_FILTER_ANISOTROPIC;
			case Nexus::Graphics::SamplerFilter::MinPoint_MagPoint_MipPoint: return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			case Nexus::Graphics::SamplerFilter::MinPoint_MagPoint_MipLinear: return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
			case Nexus::Graphics::SamplerFilter::MinPoint_MagLinear_MipPoint: return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
			case Nexus::Graphics::SamplerFilter::MinPoint_MagLinear_MipLinear: return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
			case Nexus::Graphics::SamplerFilter::MinLinear_MagPoint_MipPoint: return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
			case Nexus::Graphics::SamplerFilter::MinLinear_MagPoint_MipLinear: return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
			case Nexus::Graphics::SamplerFilter::MinLinear_MagLinear_MipPoint: return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			case Nexus::Graphics::SamplerFilter::MinLinear_MagLinear_MipLinear: return D3D12_FILTER_MIN_MAG_MIP_LINEAR;

			default: throw std::runtime_error("Failed to find a valid filter");
		}

		return {};
	}

	D3D12_TEXTURE_ADDRESS_MODE
	GetD3D12TextureAddressMode(Nexus::Graphics::SamplerAddressMode addressMode)
	{
		switch (addressMode)
		{
			case Nexus::Graphics::SamplerAddressMode::Border: return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			case Nexus::Graphics::SamplerAddressMode::Clamp: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			case Nexus::Graphics::SamplerAddressMode::Mirror: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
			case Nexus::Graphics::SamplerAddressMode::MirrorOnce: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
			case Nexus::Graphics::SamplerAddressMode::Wrap: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			default: throw std::runtime_error("Failed to find a valid sampler address mode");
		}
	}

	DXGI_FORMAT
	GetD3D12IndexBufferFormat(Nexus::Graphics::IndexBufferFormat format)
	{
		switch (format)
		{
			case Nexus::Graphics::IndexBufferFormat::UInt16: return DXGI_FORMAT_R16_UINT;
			case Nexus::Graphics::IndexBufferFormat::UInt32: return DXGI_FORMAT_R32_UINT;
			default: throw std::runtime_error("Invalid index buffer format entered");
		}
	}

	D3D12_PRIMITIVE_TOPOLOGY_TYPE
	GetPipelineTopology(Nexus::Graphics::Topology topology)
	{
		switch (topology)
		{
			case Nexus::Graphics::Topology::LineList: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
			case Nexus::Graphics::Topology::LineStrip: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
			case Nexus::Graphics::Topology::PointList: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
			case Nexus::Graphics::Topology::TriangleList: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			case Nexus::Graphics::Topology::TriangleStrip: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			default: throw std::runtime_error("Could not find a valid topology");
		}
	}

	D3D12_HEAP_TYPE GetHeapType(const Graphics::DeviceBufferDescription &desc)
	{
		switch (desc.Access)
		{
			case Graphics::BufferMemoryAccess::Upload: return D3D12_HEAP_TYPE_UPLOAD;
			case Graphics::BufferMemoryAccess::Default: return D3D12_HEAP_TYPE_DEFAULT;
			case Graphics::BufferMemoryAccess::Readback: return D3D12_HEAP_TYPE_READBACK;
			default: throw std::runtime_error("Failed to find a valid heap type");
		}
	}

	D3D12_RESOURCE_DIMENSION GetResourceDimensions(Nexus::Graphics::TextureType textureType)
	{
		switch (textureType)
		{
			case Nexus::Graphics::TextureType::Texture1D: return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
			case Nexus::Graphics::TextureType::Texture2D: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			case Nexus::Graphics::TextureType::Texture3D: return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
			default: throw std::runtime_error("Failed to find a valid resource dimension");
		}
	}

	D3D12_RESOURCE_FLAGS GetResourceFlags(uint8_t textureUsage)
	{
		D3D12_RESOURCE_FLAGS flags = {};

		if (textureUsage & Graphics::TextureUsage_RenderTarget)
		{
			flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}

		if (textureUsage & Graphics::TextureUsage_DepthStencil)
		{
			flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		}

		if (textureUsage & Graphics::TextureUsage_Storage)
		{
			flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		return flags;
	}

	D3D12_BARRIER_SYNC GetBarrierSyncType(Nexus::Graphics::BarrierStage stage)
	{
		switch (stage)
		{
			case Nexus::Graphics::BarrierStage::None: return D3D12_BARRIER_SYNC_NONE;
			case Nexus::Graphics::BarrierStage::All: return D3D12_BARRIER_SYNC_ALL;
			case Nexus::Graphics::BarrierStage::Graphics: return D3D12_BARRIER_SYNC_ALL_SHADING;
			case Nexus::Graphics::BarrierStage::VertexInput: return D3D12_BARRIER_SYNC_INPUT_ASSEMBLER;
			case Nexus::Graphics::BarrierStage::VertexShader: return D3D12_BARRIER_SYNC_VERTEX_SHADING;
			case Nexus::Graphics::BarrierStage::FragmentShader: return D3D12_BARRIER_SYNC_PIXEL_SHADING;
			case Nexus::Graphics::BarrierStage::TesselationControlShader:
			case Nexus::Graphics::BarrierStage::TesselationEvaluationShader:
			case Nexus::Graphics::BarrierStage::GeometryShader: return D3D12_BARRIER_SYNC_NON_PIXEL_SHADING;
			case Nexus::Graphics::BarrierStage::ComputeShader: return D3D12_BARRIER_SYNC_COMPUTE_SHADING;
			case Nexus::Graphics::BarrierStage::RenderTarget: return D3D12_BARRIER_SYNC_RENDER_TARGET | D3D12_BARRIER_SYNC_DEPTH_STENCIL;
			case Nexus::Graphics::BarrierStage::TransferSource: return D3D12_BARRIER_SYNC_COPY;
			case Nexus::Graphics::BarrierStage::TransferDestination: return D3D12_BARRIER_SYNC_COPY;
			case Nexus::Graphics::BarrierStage::Resolve: return D3D12_BARRIER_SYNC_RESOLVE;
			default: throw std::runtime_error("Failed to find a valid barrier sync type");
		}
	}

	D3D12_BARRIER_ACCESS GetBarrierAccessType(Nexus::Graphics::BarrierAccess access)
	{
		switch (access)
		{
			case Nexus::Graphics::BarrierAccess::None: return D3D12_BARRIER_ACCESS_NO_ACCESS;
			case Nexus::Graphics::BarrierAccess::All: return D3D12_BARRIER_ACCESS_COMMON;
			case Nexus::Graphics::BarrierAccess::VertexBuffer: return D3D12_BARRIER_ACCESS_VERTEX_BUFFER;
			case Nexus::Graphics::BarrierAccess::IndexBuffer: return D3D12_BARRIER_ACCESS_INDEX_BUFFER;
			case Nexus::Graphics::BarrierAccess::RenderTarget: return D3D12_BARRIER_ACCESS_RENDER_TARGET;
			case Nexus::Graphics::BarrierAccess::DepthStencilRead: return D3D12_BARRIER_ACCESS_DEPTH_STENCIL_READ;
			case Nexus::Graphics::BarrierAccess::DepthStencilWrite: return D3D12_BARRIER_ACCESS_DEPTH_STENCIL_WRITE;
			case Nexus::Graphics::BarrierAccess::ResolveSource: return D3D12_BARRIER_ACCESS_RESOLVE_SOURCE;
			case Nexus::Graphics::BarrierAccess::ResolveDestination: return D3D12_BARRIER_ACCESS_RESOLVE_DEST;
			case Nexus::Graphics::BarrierAccess::CopySource: return D3D12_BARRIER_ACCESS_COPY_SOURCE;
			case Nexus::Graphics::BarrierAccess::CopyDestination: return D3D12_BARRIER_ACCESS_COPY_DEST;
			case Nexus::Graphics::BarrierAccess::DrawIndirect: return D3D12_BARRIER_ACCESS_INDIRECT_ARGUMENT;
			default: throw std::runtime_error("Failed to find a valid barrier access type");
		}
	}

	D3D12_BARRIER_LAYOUT GetBarrierLayout(Nexus::Graphics::BarrierLayout layout)
	{
		switch (layout)
		{
			case Nexus::Graphics::BarrierLayout::Undefined: return D3D12_BARRIER_LAYOUT_UNDEFINED;
			case Nexus::Graphics::BarrierLayout::General: return D3D12_BARRIER_LAYOUT_COMMON;
			case Nexus::Graphics::BarrierLayout::Present: return D3D12_BARRIER_LAYOUT_PRESENT;
			case Nexus::Graphics::BarrierLayout::RenderTarget: return D3D12_BARRIER_LAYOUT_RENDER_TARGET;
			case Nexus::Graphics::BarrierLayout::DepthStencilRead: return D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_READ;
			case Nexus::Graphics::BarrierLayout::DepthStencilWrite: return D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_WRITE;
			case Nexus::Graphics::BarrierLayout::CopySource: return D3D12_BARRIER_LAYOUT_COPY_SOURCE;
			case Nexus::Graphics::BarrierLayout::CopyDestination: return D3D12_BARRIER_LAYOUT_COPY_DEST;
			case Nexus::Graphics::BarrierLayout::ResolveSource: return D3D12_BARRIER_LAYOUT_RESOLVE_SOURCE;
			case Nexus::Graphics::BarrierLayout::ResolveDestimation: return D3D12_BARRIER_LAYOUT_RESOLVE_DEST;
			case Nexus::Graphics::BarrierLayout::ShaderReadOnly: return D3D12_BARRIER_LAYOUT_SHADER_RESOURCE;
			case Nexus::Graphics::BarrierLayout::ShaderReadWrite: return D3D12_BARRIER_LAYOUT_UNORDERED_ACCESS;
			default: throw std::runtime_error("Failed to find a valid barrier layout");
		}
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC CreateTextureSrvView(const Graphics::TextureSpecification &spec)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping			= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		switch (spec.Type)
		{
			case Graphics::TextureType::Texture1D:
				if (spec.ArrayLayers > 1)
				{
					srvDesc.ViewDimension				   = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
					srvDesc.Texture1DArray.MipLevels	   = spec.MipLevels;
					srvDesc.Texture1DArray.MostDetailedMip = 0;
					srvDesc.Texture1DArray.FirstArraySlice = spec.Depth;
					srvDesc.Texture1DArray.ArraySize	   = spec.ArrayLayers;
				}
				else
				{
					srvDesc.ViewDimension			  = D3D12_SRV_DIMENSION_TEXTURE1D;
					srvDesc.Texture1D.MostDetailedMip = 0;
					srvDesc.Texture1D.MipLevels		  = spec.MipLevels;
				}
				break;

			case Graphics::TextureType::Texture2D:
			{
				if (spec.Usage & Graphics::TextureUsage_Cubemap)
				{
					if (spec.ArrayLayers == 6)
					{
						srvDesc.ViewDimension				= D3D12_SRV_DIMENSION_TEXTURECUBE;
						srvDesc.TextureCube.MostDetailedMip = 0;
						srvDesc.TextureCube.MipLevels		= spec.MipLevels;
					}
					else
					{
						srvDesc.ViewDimension					  = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
						srvDesc.TextureCubeArray.First2DArrayFace = spec.Depth;
						srvDesc.TextureCubeArray.NumCubes		  = spec.ArrayLayers / 6;
						srvDesc.TextureCubeArray.MostDetailedMip  = 0;
						srvDesc.TextureCubeArray.MipLevels		  = spec.MipLevels;
					}
				}
				else
				{
					if (spec.ArrayLayers > 1)
					{
						if (spec.Samples > 1)
						{
							srvDesc.ViewDimension					 = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
							srvDesc.Texture2DMSArray.FirstArraySlice = spec.Depth;
							srvDesc.Texture2DMSArray.ArraySize		 = spec.ArrayLayers;
						}
						else
						{
							srvDesc.ViewDimension				   = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
							srvDesc.Texture2DArray.MostDetailedMip = 0;
							srvDesc.Texture2DArray.FirstArraySlice = spec.Depth;
							srvDesc.Texture2DArray.ArraySize	   = spec.ArrayLayers;
							srvDesc.Texture2DArray.MipLevels	   = spec.MipLevels;
						}
					}
					else
					{
						if (spec.Samples > 1)
						{
							srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
						}
						else
						{
							srvDesc.ViewDimension			  = D3D12_SRV_DIMENSION_TEXTURE2D;
							srvDesc.Texture2D.MostDetailedMip = 0;
							srvDesc.Texture2D.MipLevels		  = spec.MipLevels;
						}
					}
				}

				break;
			}
			case Graphics::TextureType::Texture3D:
			{
				srvDesc.ViewDimension			  = D3D12_SRV_DIMENSION_TEXTURE3D;
				srvDesc.Texture3D.MostDetailedMip = 0;
				srvDesc.Texture3D.MipLevels		  = spec.MipLevels;

				break;
			}
			default: throw std::runtime_error("Failed to find a valid TextureType");
		}

		return srvDesc;
	}

	D3D12_UNORDERED_ACCESS_VIEW_DESC CreateTextureUavView(const Graphics::StorageImageView &view)
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC uav = {};

		Ref<Graphics::Texture>				  texture = view.TextureHandle;
		const Graphics::TextureSpecification &spec	  = texture->GetSpecification();
		uav.Format									  = D3D12::GetD3D12PixelFormat(spec.Format, false);

		switch (spec.Type)
		{
			case Graphics::TextureType::Texture1D:
				if (spec.ArrayLayers > 1)
				{
					uav.ViewDimension	   = D3D12_UAV_DIMENSION_TEXTURE1D;
					uav.Texture1D.MipSlice = view.MipLevel;
				}
				else
				{
					uav.ViewDimension				   = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
					uav.Texture1DArray.FirstArraySlice = view.ArrayLayer;
					uav.Texture1DArray.ArraySize	   = 1;
					uav.Texture1DArray.MipSlice		   = view.MipLevel;
				}
				break;

			case Graphics::TextureType::Texture2D:
			{
				if (spec.ArrayLayers > 1)
				{
					uav.ViewDimension				   = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
					uav.Texture2DArray.FirstArraySlice = view.ArrayLayer;
					uav.Texture2DArray.ArraySize	   = 1;
					uav.Texture2DArray.MipSlice		   = view.MipLevel;
					uav.Texture2DArray.PlaneSlice	   = 0;
				}
				else
				{
					uav.ViewDimension		 = D3D12_UAV_DIMENSION_TEXTURE2D;
					uav.Texture2D.MipSlice	 = view.MipLevel;
					uav.Texture2D.PlaneSlice = 0;
				}

				break;
			}
			case Graphics::TextureType::Texture3D:
			{
				uav.ViewDimension		  = D3D12_UAV_DIMENSION_TEXTURE3D;
				uav.Texture3D.MipSlice	  = view.MipLevel;
				uav.Texture3D.FirstWSlice = view.ArrayLayer;
				uav.Texture3D.WSize		  = 1;

				break;
			}
			default: throw std::runtime_error("Failed to find a valid TextureType");
		}

		return uav;
	}
}	 // namespace Nexus::D3D12
#endif