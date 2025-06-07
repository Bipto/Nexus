#include "PipelineD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Utils.hpp"
	#include "FramebufferD3D12.hpp"
	#include "ShaderModuleD3D12.hpp"
	#include "SwapchainD3D12.hpp"

namespace Nexus::Graphics
{
	void CreateRootSignature(const Nexus::Graphics::ResourceSetSpecification &resourceSet,
							 ID3D12Device9									 *device,
							 Microsoft::WRL::ComPtr<ID3DBlob>				 &inRootSignatureBlob,
							 Microsoft::WRL::ComPtr<ID3D12RootSignature>	 &inRootSignature)
	{
		D3D12_ROOT_SIGNATURE_DESC desc = {};
		desc.Flags					   = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		desc.NumParameters			   = 0;
		desc.NumStaticSamplers		   = 0;

		std::vector<D3D12_DESCRIPTOR_RANGE> samplerRanges;
		std::vector<D3D12_DESCRIPTOR_RANGE> textureConstantBufferRanges;
		std::vector<D3D12_DESCRIPTOR_RANGE> storageImageRanges;
		std::vector<D3D12_DESCRIPTOR_RANGE> storageBufferRanges;

		for (size_t i = 0; i < resourceSet.SampledImages.size(); i++)
		{
			const auto &textureInfo = resourceSet.SampledImages.at(i);
			uint32_t	slot		= ResourceSet::GetLinearDescriptorSlot(textureInfo.Set, textureInfo.Binding);

			D3D12_DESCRIPTOR_RANGE samplerRange			   = {};
			samplerRange.RangeType						   = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
			samplerRange.BaseShaderRegister				   = slot;
			samplerRange.NumDescriptors					   = 1;
			samplerRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			samplerRange.RegisterSpace					   = 0;
			samplerRanges.push_back(samplerRange);

			D3D12_DESCRIPTOR_RANGE textureRange			   = {};
			textureRange.RangeType						   = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			textureRange.BaseShaderRegister				   = slot;
			textureRange.NumDescriptors					   = 1;
			textureRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			textureRange.RegisterSpace					   = 0;
			textureConstantBufferRanges.push_back(textureRange);
		}

		for (size_t i = 0; i < resourceSet.UniformBuffers.size(); i++)
		{
			const auto &uniformBufferInfo = resourceSet.UniformBuffers.at(i);
			uint32_t	slot			  = ResourceSet::GetLinearDescriptorSlot(uniformBufferInfo.Set, uniformBufferInfo.Binding);

			D3D12_DESCRIPTOR_RANGE constantBufferRange			  = {};
			constantBufferRange.RangeType						  = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			constantBufferRange.BaseShaderRegister				  = slot;
			constantBufferRange.NumDescriptors					  = 1;
			constantBufferRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			constantBufferRange.RegisterSpace					  = 0;
			textureConstantBufferRanges.push_back(constantBufferRange);
		}

		for (size_t i = 0; i < resourceSet.StorageImages.size(); i++)
		{
			const auto &storageImageInfo = resourceSet.StorageImages.at(i);
			uint32_t	slot			 = ResourceSet::GetLinearDescriptorSlot(storageImageInfo.Set, storageImageInfo.Binding);

			D3D12_DESCRIPTOR_RANGE storageImageRange			= {};
			storageImageRange.RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			storageImageRange.BaseShaderRegister				= slot;
			storageImageRange.NumDescriptors					= 1;
			storageImageRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			storageImageRange.RegisterSpace						= 0;
			storageImageRanges.push_back(storageImageRange);
		}

		for (size_t i = 0; i < resourceSet.StorageBuffers.size(); i++)
		{
			const auto &storageBufferInfo = resourceSet.StorageBuffers.at(i);
			uint32_t	slot			  = ResourceSet::GetLinearDescriptorSlot(storageBufferInfo.Set, storageBufferInfo.Binding);

			D3D12_DESCRIPTOR_RANGE storageBufferRange			 = {};
			storageBufferRange.RangeType						 = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			storageBufferRange.BaseShaderRegister				 = slot;
			storageBufferRange.NumDescriptors					 = 1;
			storageBufferRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			storageBufferRange.RegisterSpace					 = 0;
			storageBufferRanges.push_back(storageBufferRange);
		}

		std::vector<D3D12_ROOT_PARAMETER> rootParameters;

		// sampler parameter
		{
			D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable = {};
			descriptorTable.NumDescriptorRanges			= samplerRanges.size();
			descriptorTable.pDescriptorRanges			= samplerRanges.data();

			D3D12_ROOT_PARAMETER parameter = {};
			parameter.ParameterType		   = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			parameter.DescriptorTable	   = descriptorTable;
			parameter.ShaderVisibility	   = D3D12_SHADER_VISIBILITY_ALL;

			if (samplerRanges.size() > 0)
			{
				rootParameters.push_back(parameter);
			}
		}

		// texture constant buffer parameter
		{
			D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable = {};
			descriptorTable.NumDescriptorRanges			= textureConstantBufferRanges.size();
			descriptorTable.pDescriptorRanges			= textureConstantBufferRanges.data();

			D3D12_ROOT_PARAMETER parameter = {};
			parameter.ParameterType		   = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			parameter.DescriptorTable	   = descriptorTable;
			parameter.ShaderVisibility	   = D3D12_SHADER_VISIBILITY_ALL;

			if (textureConstantBufferRanges.size() > 0)
			{
				rootParameters.push_back(parameter);
			}
		}

		// storage image parameter
		{
			D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable = {};
			descriptorTable.NumDescriptorRanges			= storageImageRanges.size();
			descriptorTable.pDescriptorRanges			= storageImageRanges.data();

			D3D12_ROOT_PARAMETER parameter = {};
			parameter.ParameterType		   = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			parameter.DescriptorTable	   = descriptorTable;
			parameter.ShaderVisibility	   = D3D12_SHADER_VISIBILITY_ALL;

			if (storageImageRanges.size() > 0)
			{
				rootParameters.push_back(parameter);
			}
		}

		// storage buffer parameter
		{
			D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable = {};
			descriptorTable.NumDescriptorRanges			= storageBufferRanges.size();
			descriptorTable.pDescriptorRanges			= storageBufferRanges.data();

			D3D12_ROOT_PARAMETER parameter = {};
			parameter.ParameterType		   = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			parameter.DescriptorTable	   = descriptorTable;
			parameter.ShaderVisibility	   = D3D12_SHADER_VISIBILITY_ALL;

			if (storageBufferRanges.size() > 0)
			{
				rootParameters.push_back(parameter);
			}
		}

		desc.NumParameters = rootParameters.size();
		desc.pParameters   = rootParameters.data();

		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
		if (SUCCEEDED(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &inRootSignatureBlob, &errorBlob)))
		{
			device->CreateRootSignature(0,
										inRootSignatureBlob->GetBufferPointer(),
										inRootSignatureBlob->GetBufferSize(),
										IID_PPV_ARGS(&inRootSignature));
		}
		else
		{
			std::string errorMessage = std::string((char *)errorBlob->GetBufferPointer());
			NX_ERROR(errorMessage);
		}
	}

	GraphicsPipelineD3D12::GraphicsPipelineD3D12(ID3D12Device9 *device, const GraphicsPipelineDescription &description)
		: GraphicsPipeline(description),
		  m_Description(description)
	{
		CreateRootSignature(description.ResourceSetSpec, device, m_RootSignatureBlob, m_RootSignature);
		CreateInputLayout();
		CreatePipeline(device);
		CreatePrimitiveTopology();
	}

	GraphicsPipelineD3D12::~GraphicsPipelineD3D12()
	{
	}

	const GraphicsPipelineDescription &GraphicsPipelineD3D12::GetPipelineDescription() const
	{
		return m_Description;
	}

	Microsoft::WRL::ComPtr<ID3D12RootSignature> GraphicsPipelineD3D12::GetRootSignature()
	{
		return m_RootSignature;
	}

	Microsoft::WRL::ComPtr<ID3D12PipelineState> GraphicsPipelineD3D12::GetPipelineState()
	{
		return m_PipelineStateObject;
	}

	D3D_PRIMITIVE_TOPOLOGY GraphicsPipelineD3D12::GetD3DPrimitiveTopology()
	{
		return m_PrimitiveTopology;
	}

	void GraphicsPipelineD3D12::Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList)
	{
		commandList->OMSetDepthBounds(m_Description.DepthStencilDesc.MinDepth, m_Description.DepthStencilDesc.MaxDepth);
		commandList->SetPipelineState(m_PipelineStateObject.Get());
		commandList->SetGraphicsRootSignature(m_RootSignature.Get());
		commandList->IASetPrimitiveTopology(m_PrimitiveTopology);
	}

	void GraphicsPipelineD3D12::CreatePipeline(ID3D12Device9 *device)
	{
		std::vector<DXGI_FORMAT> rtvFormats;

		for (uint32_t index = 0; index < m_Description.ColourTargetCount; index++)
		{
			DXGI_FORMAT colourFormat = D3D12::GetD3D12PixelFormat(m_Description.ColourFormats.at(index), false);
			rtvFormats.push_back(colourFormat);
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc {};
		pipelineDesc.pRootSignature					= m_RootSignature.Get();
		pipelineDesc.InputLayout.NumElements		= m_InputLayout.size();
		pipelineDesc.InputLayout.pInputElementDescs = m_InputLayout.data();
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

		if (m_Description.FragmentModule)
		{
			auto d3d12FragmentModule = std::dynamic_pointer_cast<ShaderModuleD3D12>(m_Description.FragmentModule);
			NX_ASSERT(d3d12FragmentModule->GetShaderStage() == ShaderStage::Fragment, "Shader module is not a fragment shader");
			auto blob = d3d12FragmentModule->GetBlob();

			pipelineDesc.PS.BytecodeLength	= blob->GetBufferSize();
			pipelineDesc.PS.pShaderBytecode = blob->GetBufferPointer();
		}

		if (m_Description.GeometryModule)
		{
			auto d3d12GeometryModule = std::dynamic_pointer_cast<ShaderModuleD3D12>(m_Description.GeometryModule);
			NX_ASSERT(d3d12GeometryModule->GetShaderStage() == ShaderStage::Geometry, "Shader module is not a geometry shader");
			auto blob = d3d12GeometryModule->GetBlob();

			pipelineDesc.GS.BytecodeLength	= blob->GetBufferSize();
			pipelineDesc.GS.pShaderBytecode = blob->GetBufferPointer();
		}

		if (m_Description.TesselationControlModule)
		{
			auto d3d12TesselationControlModule = std::dynamic_pointer_cast<ShaderModuleD3D12>(m_Description.TesselationControlModule);
			NX_ASSERT(d3d12TesselationControlModule->GetShaderStage() == ShaderStage::TesselationControl,
					  "Shader module is not a tesselation control shader");
			auto blob = d3d12TesselationControlModule->GetBlob();

			pipelineDesc.HS.BytecodeLength	= blob->GetBufferSize();
			pipelineDesc.HS.pShaderBytecode = blob->GetBufferPointer();
		}

		if (m_Description.TesselationEvaluationModule)
		{
			auto d3d12TesselationEvaluationModule = std::dynamic_pointer_cast<ShaderModuleD3D12>(m_Description.TesselationEvaluationModule);
			NX_ASSERT(d3d12TesselationEvaluationModule->GetShaderStage() == ShaderStage::TesselationEvaluation,
					  "Shader module is not a tesselation evaluation shader");
			auto blob = d3d12TesselationEvaluationModule->GetBlob();

			pipelineDesc.DS.BytecodeLength	= blob->GetBufferSize();
			pipelineDesc.DS.pShaderBytecode = blob->GetBufferPointer();
		}

		if (m_Description.VertexModule)
		{
			auto d3d12VertexModule = std::dynamic_pointer_cast<ShaderModuleD3D12>(m_Description.VertexModule);
			NX_ASSERT(d3d12VertexModule->GetShaderStage() == ShaderStage::Vertex, "Shader module is not a vertex shader");
			auto blob = d3d12VertexModule->GetBlob();

			pipelineDesc.VS.BytecodeLength	= blob->GetBufferSize();
			pipelineDesc.VS.pShaderBytecode = blob->GetBufferPointer();
		}

		pipelineDesc.PrimitiveTopologyType = D3D12::GetPipelineTopology(m_Description.PrimitiveTopology);
		pipelineDesc.RasterizerState	   = CreateRasterizerState();
		pipelineDesc.StreamOutput		   = CreateStreamOutputDesc();
		pipelineDesc.NumRenderTargets	   = rtvFormats.size();

		for (uint32_t rtvIndex = 0; rtvIndex < rtvFormats.size(); rtvIndex++) { pipelineDesc.RTVFormats[rtvIndex] = rtvFormats.at(rtvIndex); }

		DXGI_FORMAT depthFormat						 = D3D12::GetD3D12PixelFormat(m_Description.DepthFormat, true);
		pipelineDesc.DSVFormat						 = depthFormat;
		pipelineDesc.BlendState						 = CreateBlendStateDesc();
		pipelineDesc.DepthStencilState				 = CreateDepthStencilDesc();
		pipelineDesc.SampleMask						 = 0xFFFFFFFF;
		pipelineDesc.SampleDesc.Count				 = m_Description.ColourTargetSampleCount;
		pipelineDesc.SampleDesc.Quality				 = 0;
		pipelineDesc.NodeMask						 = 0;
		pipelineDesc.CachedPSO.CachedBlobSizeInBytes = 0;
		pipelineDesc.CachedPSO.pCachedBlob			 = nullptr;
		pipelineDesc.Flags							 = D3D12_PIPELINE_STATE_FLAG_NONE;

		HRESULT hr = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&m_PipelineStateObject));
		if (FAILED(hr))
		{
			_com_error error(hr);
			std::string message = "Failed to create pipeline state: " + std::string(error.ErrorMessage());
			NX_ERROR(message);
		}
	}

	void GraphicsPipelineD3D12::CreateInputLayout()
	{
		m_InputLayout.clear();

		uint32_t elementIndex = 0;
		for (uint32_t layoutIndex = 0; layoutIndex < m_Description.Layouts.size(); layoutIndex++)
		{
			const auto &layout = m_Description.Layouts.at(layoutIndex);

			for (uint32_t i = 0; i < layout.GetNumberOfElements(); i++)
			{
				const auto &element = layout.GetElement(i);

				D3D12_INPUT_CLASSIFICATION classification =
					(layout.GetInstanceStepRate() != 0) ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;

				D3D12_INPUT_ELEMENT_DESC desc = {element.Name.c_str(),
												 elementIndex,
												 D3D12::GetD3D12BaseType(element),
												 layoutIndex,
												 (UINT)element.Offset,
												 classification,
												 layout.GetInstanceStepRate()};

				m_InputLayout.push_back(desc);
				elementIndex++;
			}
		}
	}

	void GraphicsPipelineD3D12::CreatePrimitiveTopology()
	{
		switch (m_Description.PrimitiveTopology)
		{
			case Topology::LineList: m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST; break;
			case Topology::LineStrip: m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP; break;
			case Topology::PointList: m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST; break;
			case Topology::TriangleList: m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;
			case Topology::TriangleStrip: m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
		}
	}

	D3D12_RASTERIZER_DESC GraphicsPipelineD3D12::CreateRasterizerState()
	{
		D3D12_RASTERIZER_DESC desc {};
		desc.FillMode = D3D12_FILL_MODE_SOLID;
		desc.CullMode = D3D12::GetCullMode(m_Description.RasterizerStateDesc.TriangleCullMode);

		if (m_Description.RasterizerStateDesc.TriangleFrontFace == Nexus::Graphics::FrontFace::CounterClockwise)
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

	D3D12_STREAM_OUTPUT_DESC GraphicsPipelineD3D12::CreateStreamOutputDesc()
	{
		D3D12_STREAM_OUTPUT_DESC desc {};
		desc.NumEntries		  = 0;
		desc.NumStrides		  = 0;
		desc.pBufferStrides	  = nullptr;
		desc.RasterizedStream = 0;
		return desc;
	}

	D3D12_BLEND_DESC GraphicsPipelineD3D12::CreateBlendStateDesc()
	{
		D3D12_BLEND_DESC desc {};
		desc.AlphaToCoverageEnable	= FALSE;
		desc.IndependentBlendEnable = TRUE;

		for (size_t i = 0; i < m_Description.ColourBlendStates.size(); i++)
		{
			desc.RenderTarget[i].BlendEnable	= m_Description.ColourBlendStates[i].EnableBlending;
			desc.RenderTarget[i].SrcBlend		= D3D12::GetBlendFunction(m_Description.ColourBlendStates[i].SourceColourBlend);
			desc.RenderTarget[i].SrcBlend		= D3D12::GetBlendFunction(m_Description.ColourBlendStates[i].SourceColourBlend);
			desc.RenderTarget[i].DestBlend		= D3D12::GetBlendFunction(m_Description.ColourBlendStates[i].DestinationColourBlend);
			desc.RenderTarget[i].BlendOp		= D3D12::GetBlendEquation(m_Description.ColourBlendStates[i].ColorBlendFunction);
			desc.RenderTarget[i].SrcBlendAlpha	= D3D12::GetBlendFunction(m_Description.ColourBlendStates[i].SourceAlphaBlend);
			desc.RenderTarget[i].DestBlendAlpha = D3D12::GetBlendFunction(m_Description.ColourBlendStates[i].DestinationAlphaBlend);
			desc.RenderTarget[i].BlendOpAlpha	= D3D12::GetBlendEquation(m_Description.ColourBlendStates[i].AlphaBlendFunction);
			desc.RenderTarget[i].LogicOpEnable	= FALSE;
			desc.RenderTarget[i].LogicOp		= D3D12_LOGIC_OP_NOOP;

			uint8_t writeMask = 0;
			if (m_Description.ColourBlendStates[i].PixelWriteMask.Red)
			{
				writeMask |= D3D12_COLOR_WRITE_ENABLE_RED;
			}
			if (m_Description.ColourBlendStates[i].PixelWriteMask.Green)
			{
				writeMask |= D3D12_COLOR_WRITE_ENABLE_GREEN;
			}
			if (m_Description.ColourBlendStates[i].PixelWriteMask.Blue)
			{
				writeMask |= D3D12_COLOR_WRITE_ENABLE_BLUE;
			}
			if (m_Description.ColourBlendStates[i].PixelWriteMask.Alpha)
			{
				writeMask |= D3D12_COLOR_WRITE_ENABLE_ALPHA;
			}

			desc.RenderTarget[i].RenderTargetWriteMask = writeMask;
		}

		return desc;
	}

	D3D12_DEPTH_STENCIL_DESC GraphicsPipelineD3D12::CreateDepthStencilDesc()
	{
		D3D12_DEPTH_STENCIL_DESC desc {};
		desc.DepthEnable = m_Description.DepthStencilDesc.EnableDepthTest;
		desc.DepthFunc	 = D3D12::GetComparisonFunction(m_Description.DepthStencilDesc.DepthComparisonFunction);

		if (m_Description.DepthStencilDesc.EnableDepthWrite)
		{
			desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		}
		else
		{
			desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		}

		desc.StencilEnable	  = m_Description.DepthStencilDesc.EnableStencilTest;
		desc.StencilReadMask  = m_Description.DepthStencilDesc.StencilMask;
		desc.StencilWriteMask = m_Description.DepthStencilDesc.StencilMask;

		auto stencilFailOp		= D3D12::GetStencilOperation(m_Description.DepthStencilDesc.StencilFailOperation);
		auto stencilDepthFailOp = D3D12::GetStencilOperation(m_Description.DepthStencilDesc.StencilSuccessDepthFailOperation);
		auto stencilPassOp		= D3D12::GetStencilOperation(m_Description.DepthStencilDesc.StencilSuccessDepthSuccessOperation);
		auto stencilFunc		= D3D12::GetComparisonFunction(m_Description.DepthStencilDesc.StencilComparisonFunction);

		desc.FrontFace.StencilFunc		  = stencilFunc;
		desc.FrontFace.StencilDepthFailOp = stencilDepthFailOp;
		desc.FrontFace.StencilFailOp	  = stencilFailOp;
		desc.FrontFace.StencilPassOp	  = stencilPassOp;
		desc.BackFace.StencilFunc		  = stencilFunc;
		desc.BackFace.StencilDepthFailOp  = stencilDepthFailOp;
		desc.BackFace.StencilFailOp		  = stencilFailOp;
		desc.BackFace.StencilPassOp		  = stencilPassOp;
		return desc;
	}

	ComputePipelineD3D12::ComputePipelineD3D12(ID3D12Device9 *device, const ComputePipelineDescription &description) : ComputePipeline(description)
	{
		CreateRootSignature(description.ResourceSetSpec, device, m_RootSignatureBlob, m_RootSignature);
		CreatePipeline(device);
	}

	ComputePipelineD3D12::~ComputePipelineD3D12()
	{
	}

	void ComputePipelineD3D12::Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList)
	{
		commandList->SetPipelineState(m_PipelineStateObject.Get());
		commandList->SetComputeRootSignature(m_RootSignature.Get());
	}

	void ComputePipelineD3D12::CreatePipeline(ID3D12Device9 *device)
	{
		auto d3d12ComputeShader = std::dynamic_pointer_cast<ShaderModuleD3D12>(m_Description.ComputeShader);
		NX_ASSERT(d3d12ComputeShader->GetShaderStage() == ShaderStage::Compute,
				  "Shader provided to ComputePipelineDescription is not a compute shader");

		auto blob = d3d12ComputeShader->GetBlob();

		D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature					   = m_RootSignature.Get();
		desc.CS.BytecodeLength				   = blob->GetBufferSize();
		desc.CS.pShaderBytecode				   = blob->GetBufferPointer();
		desc.NodeMask						   = 0;
		desc.CachedPSO.CachedBlobSizeInBytes   = 0;
		desc.CachedPSO.pCachedBlob			   = nullptr;
		desc.Flags							   = D3D12_PIPELINE_STATE_FLAG_NONE;

		HRESULT hr = device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&m_PipelineStateObject));
		if (FAILED(hr))
		{
			_com_error	error(hr);
			std::string message = "Failed to create pipeline state: " + std::string(error.ErrorMessage());
			NX_ERROR(message);
		}
	}
}	 // namespace Nexus::Graphics

#endif