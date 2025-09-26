#include "D3D12Utils.hpp"

#if defined(NX_PLATFORM_D3D12)

	#include "GraphicsDeviceD3D12.hpp"
	#include "Nexus-Core/nxpch.hpp"
	#include "PipelineD3D12.hpp"
	#include "ShaderModuleD3D12.hpp"
	#include "StreamStateBuilder.hpp"

namespace Nexus::D3D12
{
	DXGI_FORMAT GetD3D12PixelFormat(Nexus::Graphics::PixelFormat format)
	{
		switch (format)
		{
			case Nexus::Graphics::PixelFormat::R8_UNorm: return DXGI_FORMAT_R8_UNORM;
			case Nexus::Graphics::PixelFormat::R8_SNorm: return DXGI_FORMAT_R8_SNORM;
			case Nexus::Graphics::PixelFormat::R8_UInt: return DXGI_FORMAT_R8_UINT;
			case Nexus::Graphics::PixelFormat::R8_SInt: return DXGI_FORMAT_R8_SINT;

			case Nexus::Graphics::PixelFormat::R16_UNorm: return DXGI_FORMAT_R16_UNORM;
			case Nexus::Graphics::PixelFormat::R16_SNorm: return DXGI_FORMAT_R16_SNORM;
			case Nexus::Graphics::PixelFormat::R16_UInt: return DXGI_FORMAT_R16_UINT;
			case Nexus::Graphics::PixelFormat::R16_SInt: return DXGI_FORMAT_R16_SINT;
			case Nexus::Graphics::PixelFormat::R16_Float: return DXGI_FORMAT_R16_FLOAT;

			case Nexus::Graphics::PixelFormat::R32_UInt: return DXGI_FORMAT_R32_UINT;
			case Nexus::Graphics::PixelFormat::R32_SInt: return DXGI_FORMAT_R32_SINT;
			case Nexus::Graphics::PixelFormat::R32_Float: return DXGI_FORMAT_R32_FLOAT;

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

			case Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UNorm: return DXGI_FORMAT_R10G10B10A2_UNORM;
			case Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UInt: return DXGI_FORMAT_R10G10B10A2_UINT;
			case Nexus::Graphics::PixelFormat::R11_G11_B10_Float: return DXGI_FORMAT_R11G11B10_FLOAT;

			case Nexus::Graphics::PixelFormat::BC1_Rgb_UNorm:
			case Nexus::Graphics::PixelFormat::BC1_Rgba_UNorm: return DXGI_FORMAT_BC1_UNORM;
			case Nexus::Graphics::PixelFormat::BC1_Rgb_UNorm_SRgb:
			case Nexus::Graphics::PixelFormat::BC1_Rgba_UNorm_SRgb: return DXGI_FORMAT_BC1_UNORM_SRGB;
			case Nexus::Graphics::PixelFormat::BC2_UNorm: return DXGI_FORMAT_BC2_UNORM;
			case Nexus::Graphics::PixelFormat::BC2_UNorm_SRgb: return DXGI_FORMAT_BC2_UNORM_SRGB;
			case Nexus::Graphics::PixelFormat::BC3_UNorm: return DXGI_FORMAT_BC3_UNORM;
			case Nexus::Graphics::PixelFormat::BC3_UNorm_SRgb: return DXGI_FORMAT_BC3_UNORM_SRGB;
			case Nexus::Graphics::PixelFormat::BC4_UNorm: return DXGI_FORMAT_BC4_UNORM;
			case Nexus::Graphics::PixelFormat::BC4_SNorm: return DXGI_FORMAT_BC4_SNORM;
			case Nexus::Graphics::PixelFormat::BC5_UNorm: return DXGI_FORMAT_BC5_UNORM;
			case Nexus::Graphics::PixelFormat::BC5_SNorm: return DXGI_FORMAT_BC5_SNORM;
			case Nexus::Graphics::PixelFormat::BC7_UNorm: return DXGI_FORMAT_BC7_UNORM;
			case Nexus::Graphics::PixelFormat::BC7_UNorm_SRgb: return DXGI_FORMAT_BC7_UNORM_SRGB;

			case Nexus::Graphics::PixelFormat::D16_UNorm: return DXGI_FORMAT_D16_UNORM;
			case Nexus::Graphics::PixelFormat::D24_UNorm_S8_UInt: return DXGI_FORMAT_D24_UNORM_S8_UINT;
			case Nexus::Graphics::PixelFormat::D32_SFloat: return DXGI_FORMAT_D32_FLOAT;
			case Nexus::Graphics::PixelFormat::D32_SFloat_S8_UInt: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

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
			default: throw std::runtime_error("Invalid stencil operation entered");
		}
	}

	D3D12_FILL_MODE GetFillMode(Nexus::Graphics::FillMode fillMode)
	{
		switch (fillMode)
		{
			case Nexus::Graphics::FillMode::Solid: return D3D12_FILL_MODE_SOLID;
			case Nexus::Graphics::FillMode::Wireframe: return D3D12_FILL_MODE_WIREFRAME;
			default: throw std::runtime_error("Invalid fill mode entered");
		}
	}

	D3D12_BLEND GetBlendFunction(Nexus::Graphics::BlendFactor function)
	{
		switch (function)
		{
			case Nexus::Graphics::BlendFactor::Zero: return D3D12_BLEND_ZERO;
			case Nexus::Graphics::BlendFactor::One: return D3D12_BLEND_ONE;
			case Nexus::Graphics::BlendFactor::SourceColour: return D3D12_BLEND_SRC_COLOR;
			case Nexus::Graphics::BlendFactor::OneMinusSourceColour: return D3D12_BLEND_INV_SRC_COLOR;
			case Nexus::Graphics::BlendFactor::DestinationColour: return D3D12_BLEND_DEST_COLOR;
			case Nexus::Graphics::BlendFactor::OneMinusDestinationColour: return D3D12_BLEND_INV_DEST_COLOR;
			case Nexus::Graphics::BlendFactor::SourceAlpha: return D3D12_BLEND_SRC_ALPHA;
			case Nexus::Graphics::BlendFactor::OneMinusSourceAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
			case Nexus::Graphics::BlendFactor::DestinationAlpha: return D3D12_BLEND_DEST_ALPHA;
			case Nexus::Graphics::BlendFactor::OneMinusDestinationAlpha: return D3D12_BLEND_INV_DEST_ALPHA;
			case Nexus::Graphics::BlendFactor::FactorColour: return D3D12_BLEND_BLEND_FACTOR;
			case Nexus::Graphics::BlendFactor::OneMinusFactorColour: return D3D12_BLEND_INV_BLEND_FACTOR;
			case Nexus::Graphics::BlendFactor::FactorAlpha: return D3D12_BLEND_ALPHA_FACTOR;
			case Nexus::Graphics::BlendFactor::OneMinusFactorAlpha: return D3D12_BLEND_INV_ALPHA_FACTOR;
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
			case Nexus::Graphics::ShaderDataType::R8_UInt: return DXGI_FORMAT_R8_UINT;
			case Nexus::Graphics::ShaderDataType::R8G8_UInt: return DXGI_FORMAT_R8G8_UINT;
			case Nexus::Graphics::ShaderDataType::R8G8B8A8_UInt: return DXGI_FORMAT_R8G8B8A8_UINT;

			case Nexus::Graphics::ShaderDataType::R8_UNorm: return DXGI_FORMAT_R8_UNORM;
			case Nexus::Graphics::ShaderDataType::R8G8_UNorm: return DXGI_FORMAT_R8G8_UNORM;
			case Nexus::Graphics::ShaderDataType::R8G8B8A8_UNorm: return DXGI_FORMAT_R8G8B8A8_UNORM;

			case Nexus::Graphics::ShaderDataType::R32_SFloat: return DXGI_FORMAT_R32_FLOAT;
			case Nexus::Graphics::ShaderDataType::R32G32_SFloat: return DXGI_FORMAT_R32G32_FLOAT;
			case Nexus::Graphics::ShaderDataType::R32G32B32_SFloat: return DXGI_FORMAT_R32G32B32_FLOAT;
			case Nexus::Graphics::ShaderDataType::R32G32B32A32_SFloat: return DXGI_FORMAT_R32G32B32A32_FLOAT;

			case Nexus::Graphics::ShaderDataType::R16_SFloat: return DXGI_FORMAT_R16_FLOAT;
			case Nexus::Graphics::ShaderDataType::R16G16_SFloat: return DXGI_FORMAT_R16G16_FLOAT;
			case Nexus::Graphics::ShaderDataType::R16G16B16A16_SFloat: return DXGI_FORMAT_R16G16B16A16_FLOAT;

			case Nexus::Graphics::ShaderDataType::R32_SInt: return DXGI_FORMAT_R32_SINT;
			case Nexus::Graphics::ShaderDataType::R32G32_SInt: return DXGI_FORMAT_R32G32_SINT;
			case Nexus::Graphics::ShaderDataType::R32G32B32_SInt: return DXGI_FORMAT_R32G32B32_SINT;
			case Nexus::Graphics::ShaderDataType::R32G32B32A32_SInt: return DXGI_FORMAT_R32G32B32A32_SINT;

			case Nexus::Graphics::ShaderDataType::R8_SInt: return DXGI_FORMAT_R8_SNORM;
			case Nexus::Graphics::ShaderDataType::R8G8_SInt: return DXGI_FORMAT_R8G8_SNORM;
			case Nexus::Graphics::ShaderDataType::R8G8B8A8_SInt: return DXGI_FORMAT_R8G8B8A8_SNORM;

			case Nexus::Graphics::ShaderDataType::R8_SNorm: return DXGI_FORMAT_R8_SNORM;
			case Nexus::Graphics::ShaderDataType::R8G8_SNorm: return DXGI_FORMAT_R8G8_SNORM;
			case Nexus::Graphics::ShaderDataType::R8G8B8A8_SNorm: return DXGI_FORMAT_R8G8B8A8_SNORM;

			case Nexus::Graphics::ShaderDataType::R16_SInt: return DXGI_FORMAT_R16_UINT;
			case Nexus::Graphics::ShaderDataType::R16G16_SInt: return DXGI_FORMAT_R16G16_UINT;
			case Nexus::Graphics::ShaderDataType::R16G16B16A16_SInt: return DXGI_FORMAT_R16G16B16A16_UINT;

			case Nexus::Graphics::ShaderDataType::R16_SNorm: return DXGI_FORMAT_R16_SNORM;
			case Nexus::Graphics::ShaderDataType::R16G16_SNorm: return DXGI_FORMAT_R16G16_SNORM;
			case Nexus::Graphics::ShaderDataType::R16G16B16A16_SNorm: return DXGI_FORMAT_R16G16B16A16_SNORM;

			case Nexus::Graphics::ShaderDataType::R32_UInt: return DXGI_FORMAT_R32_UINT;
			case Nexus::Graphics::ShaderDataType::R32G32_UInt: return DXGI_FORMAT_R32G32_UINT;
			case Nexus::Graphics::ShaderDataType::R32G32B32_UInt: return DXGI_FORMAT_R32G32B32_UINT;
			case Nexus::Graphics::ShaderDataType::R32G32B32A32_UInt: return DXGI_FORMAT_R32G32B32A32_UINT;

			case Nexus::Graphics::ShaderDataType::R16_UInt: return DXGI_FORMAT_R16_UINT;
			case Nexus::Graphics::ShaderDataType::R16G16_UInt: return DXGI_FORMAT_R16G16_UINT;
			case Nexus::Graphics::ShaderDataType::R16G16B16A16_UInt: return DXGI_FORMAT_R16G16B16A16_UINT;

			case Nexus::Graphics::ShaderDataType::R16_UNorm: return DXGI_FORMAT_R16_UNORM;
			case Nexus::Graphics::ShaderDataType::R16G16_UNorm: return DXGI_FORMAT_R16G16_UNORM;
			case Nexus::Graphics::ShaderDataType::R16G16B16A16_UNorm: return DXGI_FORMAT_R16G16B16A16_UNORM;

			case Nexus::Graphics::ShaderDataType::A2B10G10R10_UInt: return DXGI_FORMAT_R10G10B10A2_UINT;
			case Nexus::Graphics::ShaderDataType::A2B10G10R10_UNorm: return DXGI_FORMAT_R10G10B10A2_UNORM;

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
	GetD3D12IndexBufferFormat(Nexus::Graphics::IndexFormat format)
	{
		switch (format)
		{
			case Nexus::Graphics::IndexFormat::UInt16: return DXGI_FORMAT_R16_UINT;
			case Nexus::Graphics::IndexFormat::UInt32: return DXGI_FORMAT_R32_UINT;
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

	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGraphicsPipelineTraditional(Graphics::GraphicsDeviceD3D12				  *device,
																				  const Graphics::GraphicsPipelineDescription &description,
																				  Microsoft::WRL::ComPtr<ID3D12RootSignature>  rootSignature,
																				  const std::vector<D3D12_INPUT_ELEMENT_DESC> &inputLayout)
	{
		std::vector<DXGI_FORMAT> rtvFormats;

		for (uint32_t index = 0; index < description.ColourTargetCount; index++)
		{
			DXGI_FORMAT colourFormat = D3D12::GetD3D12PixelFormat(description.ColourFormats.at(index));
			rtvFormats.push_back(colourFormat);
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc {};
		pipelineDesc.pRootSignature					= rootSignature.Get();
		pipelineDesc.InputLayout.NumElements		= inputLayout.size();
		pipelineDesc.InputLayout.pInputElementDescs = inputLayout.data();
		pipelineDesc.IBStripCutValue				= D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		// setup shaders
		pipelineDesc.VS.BytecodeLength	= 0;
		pipelineDesc.VS.pShaderBytecode = nullptr;
		pipelineDesc.PS.BytecodeLength	= 0;
		pipelineDesc.PS.pShaderBytecode = nullptr;
		pipelineDesc.DS.BytecodeLength	= 0;
		pipelineDesc.DS.pShaderBytecode = nullptr;
		pipelineDesc.HS.BytecodeLength	= 0;
		pipelineDesc.HS.pShaderBytecode = nullptr;
		pipelineDesc.GS.BytecodeLength	= 0;
		pipelineDesc.GS.pShaderBytecode = nullptr;

		if (description.FragmentModule)
		{
			auto d3d12FragmentModule = std::dynamic_pointer_cast<Graphics::ShaderModuleD3D12>(description.FragmentModule);
			NX_VALIDATE(d3d12FragmentModule->GetShaderStage() == Graphics::ShaderStage::Fragment, "Shader module is not a fragment shader");
			auto blob = d3d12FragmentModule->GetBlob();

			pipelineDesc.PS.BytecodeLength	= blob->GetBufferSize();
			pipelineDesc.PS.pShaderBytecode = blob->GetBufferPointer();
		}

		if (description.GeometryModule)
		{
			auto d3d12GeometryModule = std::dynamic_pointer_cast<Graphics::ShaderModuleD3D12>(description.GeometryModule);
			NX_VALIDATE(d3d12GeometryModule->GetShaderStage() == Graphics::ShaderStage::Geometry, "Shader module is not a geometry shader");
			auto blob = d3d12GeometryModule->GetBlob();

			pipelineDesc.GS.BytecodeLength	= blob->GetBufferSize();
			pipelineDesc.GS.pShaderBytecode = blob->GetBufferPointer();
		}

		if (description.TesselationControlModule)
		{
			auto d3d12TesselationControlModule = std::dynamic_pointer_cast<Graphics::ShaderModuleD3D12>(description.TesselationControlModule);
			NX_VALIDATE(d3d12TesselationControlModule->GetShaderStage() == Graphics::ShaderStage::TessellationControl,
						"Shader module is not a tesselation control shader");
			auto blob = d3d12TesselationControlModule->GetBlob();

			pipelineDesc.HS.BytecodeLength	= blob->GetBufferSize();
			pipelineDesc.HS.pShaderBytecode = blob->GetBufferPointer();
		}

		if (description.TesselationEvaluationModule)
		{
			auto d3d12TesselationEvaluationModule = std::dynamic_pointer_cast<Graphics::ShaderModuleD3D12>(description.TesselationEvaluationModule);
			NX_VALIDATE(d3d12TesselationEvaluationModule->GetShaderStage() == Graphics::ShaderStage::TessellationEvaluation,
						"Shader module is not a tesselation evaluation shader");
			auto blob = d3d12TesselationEvaluationModule->GetBlob();

			pipelineDesc.DS.BytecodeLength	= blob->GetBufferSize();
			pipelineDesc.DS.pShaderBytecode = blob->GetBufferPointer();
		}

		if (description.VertexModule)
		{
			auto d3d12VertexModule = std::dynamic_pointer_cast<Graphics::ShaderModuleD3D12>(description.VertexModule);
			NX_VALIDATE(d3d12VertexModule->GetShaderStage() == Graphics::ShaderStage::Vertex, "Shader module is not a vertex shader");
			auto blob = d3d12VertexModule->GetBlob();

			pipelineDesc.VS.BytecodeLength	= blob->GetBufferSize();
			pipelineDesc.VS.pShaderBytecode = blob->GetBufferPointer();
		}

		pipelineDesc.PrimitiveTopologyType = D3D12::GetPipelineTopology(description.PrimitiveTopology);
		pipelineDesc.RasterizerState	   = D3D12::CreateRasterizerState(description.RasterizerStateDesc);
		pipelineDesc.StreamOutput		   = D3D12::CreateStreamOutputDesc();
		pipelineDesc.NumRenderTargets	   = rtvFormats.size();

		for (uint32_t rtvIndex = 0; rtvIndex < rtvFormats.size(); rtvIndex++) { pipelineDesc.RTVFormats[rtvIndex] = rtvFormats.at(rtvIndex); }

		DXGI_FORMAT depthFormat						 = D3D12::GetD3D12PixelFormat(description.DepthFormat);
		pipelineDesc.DSVFormat						 = depthFormat;
		pipelineDesc.BlendState						 = D3D12::CreateBlendStateDesc(description.ColourBlendStates);
		pipelineDesc.DepthStencilState				 = D3D12::CreateDepthStencilDesc(description.DepthStencilDesc);
		pipelineDesc.SampleMask						 = 0xFFFFFFFF;
		pipelineDesc.SampleDesc.Count				 = description.ColourTargetSampleCount;
		pipelineDesc.SampleDesc.Quality				 = 0;
		pipelineDesc.NodeMask						 = 0;
		pipelineDesc.CachedPSO.CachedBlobSizeInBytes = 0;
		pipelineDesc.CachedPSO.pCachedBlob			 = nullptr;
		pipelineDesc.Flags							 = D3D12_PIPELINE_STATE_FLAG_NONE;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;

		auto	d3d12Device = device->GetD3D12Device();
		HRESULT hr			= d3d12Device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pso));
		if (FAILED(hr))
		{
			_com_error	error(hr);
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

	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGraphicsPipelineStream(Graphics::GraphicsDeviceD3D12				 *device,
																			 const Graphics::GraphicsPipelineDescription &description,
																			 Microsoft::WRL::ComPtr<ID3D12RootSignature>  rootSignature,
																			 const std::vector<D3D12_INPUT_ELEMENT_DESC> &inputLayout)
	{
		D3D12::StreamStateBuilder builder;
		ID3D12RootSignature		 *rootSignaturePtr = rootSignature.Get();
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE, rootSignaturePtr);

		if (description.VertexModule)
		{
			auto d3d12VertexModule = std::dynamic_pointer_cast<Graphics::ShaderModuleD3D12>(description.VertexModule);
			NX_VALIDATE(d3d12VertexModule->GetShaderStage() == Graphics::ShaderStage::Vertex, "Shader module is not a vertex shader");
			auto blob = d3d12VertexModule->GetBlob();

			D3D12_SHADER_BYTECODE byteCode = {};
			byteCode.pShaderBytecode	   = blob->GetBufferPointer();
			byteCode.BytecodeLength		   = blob->GetBufferSize();
			builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS, byteCode);
		}

		if (description.FragmentModule)
		{
			auto d3d12FragmentModule = std::dynamic_pointer_cast<Graphics::ShaderModuleD3D12>(description.FragmentModule);
			NX_VALIDATE(d3d12FragmentModule->GetShaderStage() == Graphics::ShaderStage::Fragment, "Shader module is not a fragment shader");
			auto blob = d3d12FragmentModule->GetBlob();

			D3D12_SHADER_BYTECODE byteCode = {};
			byteCode.pShaderBytecode	   = blob->GetBufferPointer();
			byteCode.BytecodeLength		   = blob->GetBufferSize();
			builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS, byteCode);
		}

		if (description.TesselationEvaluationModule)
		{
			auto d3d12TessellationEvaluationModule = std::dynamic_pointer_cast<Graphics::ShaderModuleD3D12>(description.TesselationEvaluationModule);
			NX_VALIDATE(d3d12TessellationEvaluationModule->GetShaderStage() == Graphics::ShaderStage::TessellationEvaluation,
						"Shader module is not a tessellation evaluation shader");
			auto blob = d3d12TessellationEvaluationModule->GetBlob();

			D3D12_SHADER_BYTECODE byteCode = {};
			byteCode.pShaderBytecode	   = blob->GetBufferPointer();
			byteCode.BytecodeLength		   = blob->GetBufferSize();
			builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DS, byteCode);
		}

		if (description.TesselationControlModule)
		{
			auto d3d12TessellationControlModule = std::dynamic_pointer_cast<Graphics::ShaderModuleD3D12>(description.TesselationControlModule);
			NX_VALIDATE(d3d12TessellationControlModule->GetShaderStage() == Graphics::ShaderStage::TessellationControl,
						"Shader module is not a tessellation control shader");
			auto blob = d3d12TessellationControlModule->GetBlob();

			D3D12_SHADER_BYTECODE byteCode = {};
			byteCode.pShaderBytecode	   = blob->GetBufferPointer();
			byteCode.BytecodeLength		   = blob->GetBufferSize();
			builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_HS, byteCode);
		}

		if (description.GeometryModule)
		{
			auto d3d12GeometryModule = std::dynamic_pointer_cast<Graphics::ShaderModuleD3D12>(description.GeometryModule);
			NX_VALIDATE(d3d12GeometryModule->GetShaderStage() == Graphics::ShaderStage::Geometry, "Shader module is not a geometry shader");
			auto blob = d3d12GeometryModule->GetBlob();

			D3D12_SHADER_BYTECODE byteCode = {};
			byteCode.pShaderBytecode	   = blob->GetBufferPointer();
			byteCode.BytecodeLength		   = blob->GetBufferSize();
			builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_GS, byteCode);
		}

		D3D12_BLEND_DESC blendDesc = D3D12::CreateBlendStateDesc(description.ColourBlendStates);
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND, blendDesc);

		UINT sampleMask = UINT_MAX;
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK, sampleMask);

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

		std::vector<D3D12_INPUT_ELEMENT_DESC> layout		  = inputLayout;
		D3D12_INPUT_LAYOUT_DESC				  inputLayoutDesc = {};
		inputLayoutDesc.pInputElementDescs					  = layout.data();
		inputLayoutDesc.NumElements							  = (UINT)layout.size();
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT, inputLayoutDesc);

		D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopology = D3D12::GetPipelineTopology(description.PrimitiveTopology);
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY, primitiveTopology);

		D3D12_RT_FORMAT_ARRAY rtFormats = {};
		rtFormats.NumRenderTargets		= description.ColourTargetCount;
		for (uint32_t index = 0; index < description.ColourTargetCount; index++)
		{
			rtFormats.RTFormats[index] = D3D12::GetD3D12PixelFormat(description.ColourFormats[index]);
		}
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS, rtFormats);

		DXGI_FORMAT depthFormat = D3D12::GetD3D12PixelFormat(description.DepthFormat);
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT, depthFormat);

		DXGI_SAMPLE_DESC sampleDesc = {};
		sampleDesc.Count			= description.ColourTargetSampleCount;
		sampleDesc.Quality			= 0;
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC, sampleDesc);

		UINT nodeMask = 0;
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_NODE_MASK, nodeMask);

		D3D12_PIPELINE_STATE_FLAGS flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_FLAGS, flags);

		Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
		auto										d3d12Device = device->GetD3D12Device();

		D3D12_PIPELINE_STATE_STREAM_DESC desc = {};
		desc.pPipelineStateSubobjectStream	  = builder.GetStream();
		desc.SizeInBytes					  = builder.GetSizeInBytes();

		HRESULT hr = d3d12Device->CreatePipelineState(&desc, IID_PPV_ARGS(&pso));
		if (FAILED(hr))
		{
			_com_error	error(hr);
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

	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGraphicsPipeline(Graphics::GraphicsDeviceD3D12			   *device,
																	   const Graphics::GraphicsPipelineDescription &description,
																	   Microsoft::WRL::ComPtr<ID3D12RootSignature>	rootSignature,
																	   const std::vector<D3D12_INPUT_ELEMENT_DESC> &inputLayout)
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

	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateComputePipelineStream(Graphics::GraphicsDeviceD3D12			   *device,
																			const Graphics::ComputePipelineDescription &description,
																			Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature)
	{
		auto d3d12ComputeShader = std::dynamic_pointer_cast<Graphics::ShaderModuleD3D12>(description.ComputeShader);
		NX_VALIDATE(d3d12ComputeShader->GetShaderStage() == Graphics::ShaderStage::Compute,
					"Shader provided to ComputePipelineDescription is not a compute shader");

		auto blob = d3d12ComputeShader->GetBlob();

		D3D12::StreamStateBuilder builder;
		ID3D12RootSignature		 *rootSignaturePtr = rootSignature.Get();
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE, rootSignaturePtr);

		D3D12_SHADER_BYTECODE byteCode = {};
		byteCode.pShaderBytecode	   = blob->GetBufferPointer();
		byteCode.BytecodeLength		   = blob->GetBufferSize();
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS, byteCode);

		UINT nodeMask = 0;
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_NODE_MASK, nodeMask);

		D3D12_PIPELINE_STATE_FLAGS flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_FLAGS, flags);

		Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
		auto										d3d12Device = device->GetD3D12Device();

		D3D12_PIPELINE_STATE_STREAM_DESC desc = {};
		desc.pPipelineStateSubobjectStream	  = builder.GetStream();
		desc.SizeInBytes					  = builder.GetSizeInBytes();

		HRESULT hr = d3d12Device->CreatePipelineState(&desc, IID_PPV_ARGS(&pso));
		if (FAILED(hr))
		{
			_com_error	error(hr);
			std::string message = "Failed to create pipeline state: " + std::string(error.ErrorMessage());
			NX_ERROR(message);
		}

		std::wstring debugName = {description.DebugName.begin(), description.DebugName.end()};
		pso->SetName(debugName.c_str());

		return pso;
	}

	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateComputePipelineTraditional(Graphics::GraphicsDeviceD3D12				*device,
																				 const Graphics::ComputePipelineDescription &description,
																				 Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature)
	{
		auto d3d12ComputeShader = std::dynamic_pointer_cast<Graphics::ShaderModuleD3D12>(description.ComputeShader);
		NX_VALIDATE(d3d12ComputeShader->GetShaderStage() == Graphics::ShaderStage::Compute,
					"Shader provided to ComputePipelineDescription is not a compute shader");

		auto blob = d3d12ComputeShader->GetBlob();

		D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature					   = rootSignature.Get();
		desc.CS.BytecodeLength				   = blob->GetBufferSize();
		desc.CS.pShaderBytecode				   = blob->GetBufferPointer();
		desc.NodeMask						   = 0;
		desc.CachedPSO.CachedBlobSizeInBytes   = 0;
		desc.CachedPSO.pCachedBlob			   = nullptr;
		desc.Flags							   = D3D12_PIPELINE_STATE_FLAG_NONE;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;

		auto	d3d12Device = device->GetD3D12Device();
		HRESULT hr			= d3d12Device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&pso));
		if (FAILED(hr))
		{
			_com_error	error(hr);
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

	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateComputePipeline(Graphics::GraphicsDeviceD3D12				 *device,
																	  const Graphics::ComputePipelineDescription &description,
																	  Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature)
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

	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateMeshletPipeline(Graphics::GraphicsDeviceD3D12				 *device,
																	  const Graphics::MeshletPipelineDescription &description,
																	  Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature)
	{
		D3D12::StreamStateBuilder builder;
		ID3D12RootSignature		 *rootSignaturePtr = rootSignature.Get();
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE, rootSignaturePtr);

		if (description.TaskModule)
		{
			auto d3d12TaskModule = std::dynamic_pointer_cast<Graphics::ShaderModuleD3D12>(description.TaskModule);
			NX_VALIDATE(d3d12TaskModule->GetShaderStage() == Graphics::ShaderStage::Task, "Shader module is not a task shader");
			auto blob = d3d12TaskModule->GetBlob();

			D3D12_SHADER_BYTECODE byteCode = {};
			byteCode.pShaderBytecode	   = blob->GetBufferPointer();
			byteCode.BytecodeLength		   = blob->GetBufferSize();
			builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_AS, byteCode);
		}

		if (description.MeshModule)
		{
			auto d3d12MeshModule = std::dynamic_pointer_cast<Graphics::ShaderModuleD3D12>(description.MeshModule);
			NX_VALIDATE(d3d12MeshModule->GetShaderStage() == Graphics::ShaderStage::Mesh, "Shader module is not a mesh shader");
			auto blob = d3d12MeshModule->GetBlob();

			D3D12_SHADER_BYTECODE byteCode = {};
			byteCode.pShaderBytecode	   = blob->GetBufferPointer();
			byteCode.BytecodeLength		   = blob->GetBufferSize();
			builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MS, byteCode);
		}

		if (description.FragmentModule)
		{
			auto d3d12FragmentModule = std::dynamic_pointer_cast<Graphics::ShaderModuleD3D12>(description.FragmentModule);
			NX_VALIDATE(d3d12FragmentModule->GetShaderStage() == Graphics::ShaderStage::Fragment, "Shader module is not a fragment shader");
			auto blob = d3d12FragmentModule->GetBlob();

			D3D12_SHADER_BYTECODE byteCode = {};
			byteCode.pShaderBytecode	   = blob->GetBufferPointer();
			byteCode.BytecodeLength		   = blob->GetBufferSize();
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

		UINT sampleMask = UINT_MAX;
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK, sampleMask);

		D3D12_RASTERIZER_DESC rasterizerDesc = D3D12::CreateRasterizerState(description.RasterizerStateDesc);
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER, rasterizerDesc);

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = D3D12::CreateDepthStencilDesc(description.DepthStencilDesc);
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL, depthStencilDesc);

		D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopology = D3D12::GetPipelineTopology(description.PrimitiveTopology);
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY, primitiveTopology);

		D3D12_RT_FORMAT_ARRAY rtFormats = {};
		rtFormats.NumRenderTargets		= description.ColourTargetCount;
		for (uint32_t index = 0; index < description.ColourTargetCount; index++)
		{
			rtFormats.RTFormats[index] = D3D12::GetD3D12PixelFormat(description.ColourFormats[index]);
		}
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS, rtFormats);

		DXGI_FORMAT depthFormat = D3D12::GetD3D12PixelFormat(description.DepthFormat);
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT, depthFormat);

		DXGI_SAMPLE_DESC sampleDesc = {};
		sampleDesc.Count			= description.ColourTargetSampleCount;
		sampleDesc.Quality			= 0;
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC, sampleDesc);

		UINT nodeMask = 0;
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_NODE_MASK, nodeMask);

		D3D12_PIPELINE_STATE_FLAGS flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		builder.AddSubObject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_FLAGS, flags);

		Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
		auto										d3d12Device = device->GetD3D12Device();

		D3D12_PIPELINE_STATE_STREAM_DESC desc = {};
		desc.pPipelineStateSubobjectStream	  = builder.GetStream();
		desc.SizeInBytes					  = builder.GetSizeInBytes();

		HRESULT hr = d3d12Device->CreatePipelineState(&desc, IID_PPV_ARGS(&pso));
		if (FAILED(hr))
		{
			_com_error	error(hr);
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
			case Nexus::Graphics::TextureType::Texture2D:
			case Nexus::Graphics::TextureType::TextureCube: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			case Nexus::Graphics::TextureType::Texture3D: return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
			default: throw std::runtime_error("Failed to find a valid resource dimension");
		}
	}

	D3D12_RESOURCE_FLAGS GetResourceFlags(Graphics::PixelFormat format, uint8_t textureUsage)
	{
		D3D12_RESOURCE_FLAGS flags = {};

		if (textureUsage & Graphics::TextureUsage_RenderTarget)
		{
			flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}

		Graphics::PixelFormatType pixelFormatType = Graphics::GetPixelFormatType(format);
		if (pixelFormatType == Graphics::PixelFormatType::DepthStencil)
		{
			flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		}

		if (textureUsage & Graphics::TextureUsage_Storage)
		{
			flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		return flags;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC CreateTextureSrvView(const Graphics::TextureDescription &spec)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping			= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		switch (spec.Type)
		{
			case Graphics::TextureType::Texture1D:
				if (spec.DepthOrArrayLayers > 1)
				{
					srvDesc.ViewDimension				   = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
					srvDesc.Texture1DArray.MipLevels	   = spec.MipLevels;
					srvDesc.Texture1DArray.MostDetailedMip = 0;
					srvDesc.Texture1DArray.FirstArraySlice = 0;
					srvDesc.Texture1DArray.ArraySize	   = spec.DepthOrArrayLayers;
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
				if (spec.DepthOrArrayLayers > 1)
				{
					if (spec.Samples > 1)
					{
						srvDesc.ViewDimension					 = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
						srvDesc.Texture2DMSArray.FirstArraySlice = 0;
						srvDesc.Texture2DMSArray.ArraySize		 = spec.DepthOrArrayLayers;
					}
					else
					{
						srvDesc.ViewDimension				   = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
						srvDesc.Texture2DArray.MostDetailedMip = 0;
						srvDesc.Texture2DArray.FirstArraySlice = 0;
						srvDesc.Texture2DArray.ArraySize	   = spec.DepthOrArrayLayers;
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

				break;
			}
			case Graphics::TextureType::Texture3D:
			{
				srvDesc.ViewDimension			  = D3D12_SRV_DIMENSION_TEXTURE3D;
				srvDesc.Texture3D.MostDetailedMip = 0;
				srvDesc.Texture3D.MipLevels		  = spec.MipLevels;
				break;
			}
			case Graphics::TextureType::TextureCube:
			{
				if (spec.DepthOrArrayLayers == 1)
				{
					srvDesc.ViewDimension				= D3D12_SRV_DIMENSION_TEXTURECUBE;
					srvDesc.TextureCube.MostDetailedMip = 0;
					srvDesc.TextureCube.MipLevels		= spec.MipLevels;
				}
				else
				{
					srvDesc.ViewDimension					  = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
					srvDesc.TextureCubeArray.First2DArrayFace = 0;
					srvDesc.TextureCubeArray.NumCubes		  = spec.DepthOrArrayLayers;
					srvDesc.TextureCubeArray.MostDetailedMip  = 0;
					srvDesc.TextureCubeArray.MipLevels		  = spec.MipLevels;
				}
				break;
			}
			default: throw std::runtime_error("Failed to find a valid TextureType");
		}

		return srvDesc;
	}

	D3D12_UNORDERED_ACCESS_VIEW_DESC CreateTextureUavView(const Graphics::StorageImageView &view)
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC uav = {};

		Ref<Graphics::Texture>				texture = view.TextureHandle;
		const Graphics::TextureDescription &spec	= texture->GetDescription();
		uav.Format									= D3D12::GetD3D12PixelFormat(spec.Format);

		switch (spec.Type)
		{
			case Graphics::TextureType::Texture1D:
				if (spec.DepthOrArrayLayers > 1)
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
				if (spec.DepthOrArrayLayers > 1)
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

	void GetShaderAccessModifiers(Graphics::StorageResourceAccess access, bool &readonly, bool &byteAddress)
	{
		switch (access)
		{
			case Graphics::StorageResourceAccess::Read:
			case Graphics::StorageResourceAccess::ReadStructured:
			{
				readonly	= true;
				byteAddress = false;
				return;
			}
			case Graphics::StorageResourceAccess::ReadByteAddress:
			{
				readonly	= true;
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
				readonly	= false;
				byteAddress = false;
				return;
			}
			case Graphics::StorageResourceAccess::ReadWriteByteAddress:
			{
				readonly	= false;
				byteAddress = true;
				return;
			}

			default: throw std::runtime_error("Failed to find a valid access modifier");
		}
	}

	D3D12_SHADER_VISIBILITY GetShaderVisibility(const Graphics::ShaderStageFlags &flags)
	{
		// if we don't specify any shader stages or we supply multiple, then we have to make it visible to all shader stages
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
					default: throw std::runtime_error("Failed to find a valid access type");
				}
			}
			default: throw std::runtime_error("Failed to find a valid descriptor range type");
		}
	}

	void CreateRootSignature(const std::map<std::string, Graphics::ShaderResource> &resources,
							 Microsoft::WRL::ComPtr<ID3D12Device9>					device,
							 Microsoft::WRL::ComPtr<ID3DBlob>					   &inRootSignatureBlob,
							 Microsoft::WRL::ComPtr<ID3D12RootSignature>		   &inRootSignature,
							 DescriptorHandleInfo								   &descriptorHandleInfo)
	{
		// a util struct to temporarily store the different types of ranges prior to creating the root signature
		struct DescriptorRangeInfo
		{
			std::vector<D3D12_DESCRIPTOR_RANGE> SamplerRanges = {};
			std::vector<D3D12_DESCRIPTOR_RANGE> OtherRanges	  = {};
		};

		// create storage for descriptor ranges and root parameters
		std::map<D3D12_SHADER_VISIBILITY, DescriptorRangeInfo> descriptorRanges;
		std::vector<D3D12_ROOT_PARAMETER>					   rootParameters;

		uint32_t samplerIndex	 = 0;
		uint32_t nonSamplerIndex = 0;

		// iterate through resources and create descriptor range
		for (const auto &[name, resourceInfo] : resources)
		{
			D3D12_SHADER_VISIBILITY		visibility	   = GetShaderVisibility(resourceInfo.Stage);
			D3D12_DESCRIPTOR_RANGE_TYPE descriptorType = GetDescriptorRangeType(resourceInfo);

			// samplers cannot share a descriptor range with other descriptors so we need them to be separate
			if (descriptorType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER)
			{
				D3D12_DESCRIPTOR_RANGE &range			= descriptorRanges[visibility].SamplerRanges.emplace_back();
				range.RangeType							= GetDescriptorRangeType(resourceInfo);
				range.BaseShaderRegister				= resourceInfo.Binding;
				range.NumDescriptors					= resourceInfo.ResourceCount;
				range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
				range.RegisterSpace						= resourceInfo.RegisterSpace;

				// retrieve and then increment offset
				descriptorHandleInfo.SamplerIndexes[resourceInfo.Name] = samplerIndex;
				samplerIndex += resourceInfo.ResourceCount;
				descriptorHandleInfo.SamplerHeapCount += resourceInfo.ResourceCount;
			}
			else
			{
				D3D12_DESCRIPTOR_RANGE &range			= descriptorRanges[visibility].OtherRanges.emplace_back();
				range.RangeType							= GetDescriptorRangeType(resourceInfo);
				range.BaseShaderRegister				= resourceInfo.Binding;
				range.NumDescriptors					= resourceInfo.ResourceCount;
				range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
				range.RegisterSpace						= resourceInfo.RegisterSpace;

				// retrieve and then increment offset
				descriptorHandleInfo.NonSamplerIndexes[resourceInfo.Name] = nonSamplerIndex;
				nonSamplerIndex += resourceInfo.ResourceCount;
				descriptorHandleInfo.SRV_UAV_CBV_HeapCount += resourceInfo.ResourceCount;

				// if the resource is a storage buffer, we need to record how to bind it correctly in D3D12, e.g. StructuredBuffer/ByteAddressBuffer
				if (resourceInfo.Type == Graphics::ResourceType::StorageBuffer)
				{
					descriptorHandleInfo.StorageBuffers[name] = resourceInfo.Access;
				}
			}
		}

		// loop through all resources to find textures
		for (const auto &[textureName, textureInfo] : resources)
		{
			// if the resource is a texture, loop through to find any samplers
			if (textureInfo.Type == Graphics::ResourceType::Texture)
			{
				for (const auto &[samplerName, samplerInfo] : resources)
				{
					// if the resource is a sampler, then we need to compare it against the texture to check if it forms a combined image sampler
					if (samplerInfo.Type == Graphics::ResourceType::Sampler)
					{
						// we have found a combined image sampler
						if (textureInfo.Binding == samplerInfo.Binding && textureInfo.ResourceCount == samplerInfo.ResourceCount)
						{
							descriptorHandleInfo.CombinedImageSamplerMap[textureName] = samplerName;
						}
					}
				}
			}
		}

		size_t currentSamplerOffset	   = 0;
		size_t currentNonSamplerOffset = 0;

		// create the descriptor tables for the ranges
		for (const auto &[shaderVisibility, descriptorRange] : descriptorRanges)
		{
			if (!descriptorRange.SamplerRanges.empty())
			{
				D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable = {};
				descriptorTable.pDescriptorRanges			= descriptorRange.SamplerRanges.data();
				descriptorTable.NumDescriptorRanges			= descriptorRange.SamplerRanges.size();

				D3D12_ROOT_PARAMETER &rootParameter = rootParameters.emplace_back();
				rootParameter.ParameterType			= D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				rootParameter.DescriptorTable		= descriptorTable;
				rootParameter.ShaderVisibility		= shaderVisibility;

				DescriptorTableInfo &descriptorTableInfo = descriptorHandleInfo.DescriptorTables.emplace_back();
				descriptorTableInfo.Source				 = DescriptorHandleSource::Sampler;
				descriptorTableInfo.Offset				 = currentSamplerOffset;

				for (const auto &range : descriptorRange.SamplerRanges) { currentSamplerOffset += range.NumDescriptors; }
			}

			if (!descriptorRange.OtherRanges.empty())
			{
				D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable = {};
				descriptorTable.pDescriptorRanges			= descriptorRange.OtherRanges.data();
				descriptorTable.NumDescriptorRanges			= descriptorRange.OtherRanges.size();

				D3D12_ROOT_PARAMETER &rootParameter = rootParameters.emplace_back();
				rootParameter.ParameterType			= D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				rootParameter.DescriptorTable		= descriptorTable;
				rootParameter.ShaderVisibility		= shaderVisibility;

				DescriptorTableInfo &descriptorTableInfo = descriptorHandleInfo.DescriptorTables.emplace_back();
				descriptorTableInfo.Source				 = DescriptorHandleSource::SRV_UAV_CBV;
				descriptorTableInfo.Offset				 = currentNonSamplerOffset;

				for (const auto &range : descriptorRange.OtherRanges) { currentNonSamplerOffset += range.NumDescriptors; }
			}
		}

		// create the D3D12 root signature
		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Flags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
		rootSignatureDesc.NumStaticSamplers = 0;
		rootSignatureDesc.pStaticSamplers	= nullptr;
		rootSignatureDesc.NumParameters		= rootParameters.size();
		rootSignatureDesc.pParameters		= rootParameters.data();

		// serialize the root signature and report any errors if they occur
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
		if (SUCCEEDED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &inRootSignatureBlob, &errorBlob)))
		{
			device->CreateRootSignature(0,
										inRootSignatureBlob->GetBufferPointer(),
										inRootSignatureBlob->GetBufferSize(),
										IID_PPV_ARGS(&inRootSignature));
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

				D3D12_INPUT_CLASSIFICATION classification =
					layout.IsInstanceBuffer() ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;

				D3D12_INPUT_ELEMENT_DESC desc =
					{element.Name.c_str(), elementIndex, D3D12::GetD3D12BaseType(element), layoutIndex, (UINT)element.Offset, classification, 0};

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
			case Graphics::Topology::LineList: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
			case Graphics::Topology::LineStrip: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
			case Graphics::Topology::PointList: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
			case Graphics::Topology::TriangleList: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			case Graphics::Topology::TriangleStrip: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			default: throw std::runtime_error("Failed to find a valid topology");
		}
	}

	D3D12_RASTERIZER_DESC CreateRasterizerState(const Graphics::RasterizerStateDescription &rasterizerState)
	{
		D3D12_RASTERIZER_DESC desc {};
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

		desc.DepthBias			   = 0;
		desc.DepthBiasClamp		   = .0f;
		desc.SlopeScaledDepthBias  = .0f;
		desc.DepthClipEnable	   = FALSE;
		desc.MultisampleEnable	   = FALSE;
		desc.AntialiasedLineEnable = FALSE;
		desc.ForcedSampleCount	   = 0;
		return desc;
	}

	D3D12_STREAM_OUTPUT_DESC CreateStreamOutputDesc()
	{
		D3D12_STREAM_OUTPUT_DESC desc {};
		desc.NumEntries		  = 0;
		desc.NumStrides		  = 0;
		desc.pBufferStrides	  = nullptr;
		desc.RasterizedStream = 0;
		return desc;
	}

	D3D12_BLEND_DESC CreateBlendStateDesc(const std::array<Graphics::BlendStateDescription, 8> &colourBlendStates)
	{
		D3D12_BLEND_DESC desc {};
		desc.AlphaToCoverageEnable	= FALSE;
		desc.IndependentBlendEnable = TRUE;

		for (size_t i = 0; i < colourBlendStates.size(); i++)
		{
			desc.RenderTarget[i].BlendEnable	= colourBlendStates[i].EnableBlending;
			desc.RenderTarget[i].SrcBlend		= D3D12::GetBlendFunction(colourBlendStates[i].SourceColourBlend);
			desc.RenderTarget[i].SrcBlend		= D3D12::GetBlendFunction(colourBlendStates[i].SourceColourBlend);
			desc.RenderTarget[i].DestBlend		= D3D12::GetBlendFunction(colourBlendStates[i].DestinationColourBlend);
			desc.RenderTarget[i].BlendOp		= D3D12::GetBlendEquation(colourBlendStates[i].ColorBlendFunction);
			desc.RenderTarget[i].SrcBlendAlpha	= D3D12::GetBlendFunction(colourBlendStates[i].SourceAlphaBlend);
			desc.RenderTarget[i].DestBlendAlpha = D3D12::GetBlendFunction(colourBlendStates[i].DestinationAlphaBlend);
			desc.RenderTarget[i].BlendOpAlpha	= D3D12::GetBlendEquation(colourBlendStates[i].AlphaBlendFunction);
			desc.RenderTarget[i].LogicOpEnable	= FALSE;
			desc.RenderTarget[i].LogicOp		= D3D12_LOGIC_OP_NOOP;

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
		D3D12_DEPTH_STENCIL_DESC desc {};
		desc.DepthEnable = depthStencilDesc.EnableDepthTest;
		desc.DepthFunc	 = D3D12::GetComparisonFunction(depthStencilDesc.DepthComparisonFunction);

		if (depthStencilDesc.EnableDepthWrite)
		{
			desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		}
		else
		{
			desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		}

		desc.StencilEnable	  = depthStencilDesc.EnableStencilTest;
		desc.StencilReadMask  = depthStencilDesc.StencilCompareMask;
		desc.StencilWriteMask = depthStencilDesc.StencilWriteMask;

		desc.FrontFace.StencilFunc		  = D3D12::GetComparisonFunction(depthStencilDesc.Front.StencilComparisonFunction);
		desc.FrontFace.StencilDepthFailOp = D3D12::GetStencilOperation(depthStencilDesc.Front.StencilSuccessDepthFailOperation);
		desc.FrontFace.StencilFailOp	  = D3D12::GetStencilOperation(depthStencilDesc.Front.StencilFailOperation);
		desc.FrontFace.StencilPassOp	  = D3D12::GetStencilOperation(depthStencilDesc.Front.StencilSuccessDepthSuccessOperation);

		desc.BackFace.StencilFunc		 = D3D12::GetComparisonFunction(depthStencilDesc.Back.StencilComparisonFunction);
		desc.BackFace.StencilDepthFailOp = D3D12::GetStencilOperation(depthStencilDesc.Back.StencilSuccessDepthFailOperation);
		desc.BackFace.StencilFailOp		 = D3D12::GetStencilOperation(depthStencilDesc.Back.StencilFailOperation);
		desc.BackFace.StencilPassOp		 = D3D12::GetStencilOperation(depthStencilDesc.Back.StencilSuccessDepthSuccessOperation);

		return desc;
	}

	D3D12_DEPTH_STENCIL_DESC1 CreateDepthStencilDesc1(const Graphics::DepthStencilDescription &depthStencilDesc)
	{
		D3D12_DEPTH_STENCIL_DESC1 desc = {};
		desc.DepthEnable			   = depthStencilDesc.EnableDepthTest;
		desc.DepthFunc				   = D3D12::GetComparisonFunction(depthStencilDesc.DepthComparisonFunction);

		if (depthStencilDesc.EnableDepthWrite)
		{
			desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		}
		else
		{
			desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		}

		desc.StencilEnable	  = depthStencilDesc.EnableStencilTest;
		desc.StencilReadMask  = depthStencilDesc.StencilCompareMask;
		desc.StencilWriteMask = depthStencilDesc.StencilWriteMask;

		desc.FrontFace.StencilFunc		  = D3D12::GetComparisonFunction(depthStencilDesc.Front.StencilComparisonFunction);
		desc.FrontFace.StencilDepthFailOp = D3D12::GetStencilOperation(depthStencilDesc.Front.StencilSuccessDepthFailOperation);
		desc.FrontFace.StencilFailOp	  = D3D12::GetStencilOperation(depthStencilDesc.Front.StencilFailOperation);
		desc.FrontFace.StencilPassOp	  = D3D12::GetStencilOperation(depthStencilDesc.Front.StencilSuccessDepthSuccessOperation);

		desc.BackFace.StencilFunc		 = D3D12::GetComparisonFunction(depthStencilDesc.Back.StencilComparisonFunction);
		desc.BackFace.StencilDepthFailOp = D3D12::GetStencilOperation(depthStencilDesc.Back.StencilSuccessDepthFailOperation);
		desc.BackFace.StencilFailOp		 = D3D12::GetStencilOperation(depthStencilDesc.Back.StencilFailOperation);
		desc.BackFace.StencilPassOp		 = D3D12::GetStencilOperation(depthStencilDesc.Back.StencilSuccessDepthSuccessOperation);

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
			default: throw std::runtime_error("Failed to find a valid present mode");
		}

		return 0;
	}

	D3D12_RESOURCE_STATES GetTextureResourceState(Graphics::TextureLayout layout)
	{
		switch (layout)
		{
			case Graphics::TextureLayout::Undefined: return D3D12_RESOURCE_STATE_COMMON;
			case Graphics::TextureLayout::General: return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			case Graphics::TextureLayout::ColourAttachmentOptimal: return D3D12_RESOURCE_STATE_RENDER_TARGET;
			case Graphics::TextureLayout::DepthStencilAttachmentOptimal: return D3D12_RESOURCE_STATE_DEPTH_WRITE;
			case Graphics::TextureLayout::DepthStencilReadOnlyOptimal: return D3D12_RESOURCE_STATE_DEPTH_READ;
			case Graphics::TextureLayout::ShaderReadOnlyOptimal: return D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
			case Graphics::TextureLayout::TransferSrcOptimal: return D3D12_RESOURCE_STATE_COPY_SOURCE;
			case Graphics::TextureLayout::TransferDstOptimal: return D3D12_RESOURCE_STATE_COPY_DEST;
			case Graphics::TextureLayout::DepthReadOnlyStencilAttachmentOptimal:
			case Graphics::TextureLayout::DepthAttachmentStencilReadOnlyOptimal:
			case Graphics::TextureLayout::DepthAttachmentOptimal: return D3D12_RESOURCE_STATE_DEPTH_WRITE;
			case Graphics::TextureLayout::DepthReadOnlyOptimal: return D3D12_RESOURCE_STATE_DEPTH_READ;
			case Graphics::TextureLayout::StencilAttachmentOptimal: return D3D12_RESOURCE_STATE_DEPTH_WRITE;
			case Graphics::TextureLayout::StencilReadOnlyOptimal: return D3D12_RESOURCE_STATE_DEPTH_READ;
			case Graphics::TextureLayout::ReadonlyOptimal: return D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
			case Graphics::TextureLayout::PresentSrc: return D3D12_RESOURCE_STATE_PRESENT;
			default: throw std::runtime_error("Failed to find a valid resource state");
		}
	}
}	 // namespace Nexus::D3D12
#endif