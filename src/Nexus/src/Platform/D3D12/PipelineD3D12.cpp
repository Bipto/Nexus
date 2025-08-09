#include "PipelineD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Utils.hpp"
	#include "FramebufferD3D12.hpp"
	#include "ShaderModuleD3D12.hpp"
	#include "SwapchainD3D12.hpp"

namespace Nexus::Graphics
{

	GraphicsPipelineD3D12::GraphicsPipelineD3D12(ID3D12Device9 *device, const GraphicsPipelineDescription &description)
		: GraphicsPipeline(description),
		  m_Description(description)
	{
		const auto &resources = GetRequiredShaderResources();
		D3D12::CreateRootSignature(resources, device, m_RootSignatureBlob, m_RootSignature);
		m_InputLayout = D3D12::CreateInputLayout(description.Layouts);
		CreatePipeline(device);
		m_PrimitiveTopology = D3D12::CreatePrimitiveTopology(description.PrimitiveTopology);
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
		commandList->SetPipelineState(m_PipelineStateObject.Get());
		commandList->SetGraphicsRootSignature(m_RootSignature.Get());
		commandList->IASetPrimitiveTopology(m_PrimitiveTopology);
	}

	void GraphicsPipelineD3D12::CreatePipeline(ID3D12Device9 *device)
	{
		std::vector<DXGI_FORMAT> rtvFormats;

		for (uint32_t index = 0; index < m_Description.ColourTargetCount; index++)
		{
			DXGI_FORMAT colourFormat = D3D12::GetD3D12PixelFormat(m_Description.ColourFormats.at(index));
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
			NX_ASSERT(d3d12TesselationControlModule->GetShaderStage() == ShaderStage::TessellationControl,
					  "Shader module is not a tesselation control shader");
			auto blob = d3d12TesselationControlModule->GetBlob();

			pipelineDesc.HS.BytecodeLength	= blob->GetBufferSize();
			pipelineDesc.HS.pShaderBytecode = blob->GetBufferPointer();
		}

		if (m_Description.TesselationEvaluationModule)
		{
			auto d3d12TesselationEvaluationModule = std::dynamic_pointer_cast<ShaderModuleD3D12>(m_Description.TesselationEvaluationModule);
			NX_ASSERT(d3d12TesselationEvaluationModule->GetShaderStage() == ShaderStage::TessellationEvaluation,
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
		pipelineDesc.RasterizerState	   = D3D12::CreateRasterizerState(m_Description.RasterizerStateDesc);
		pipelineDesc.StreamOutput		   = D3D12::CreateStreamOutputDesc();
		pipelineDesc.NumRenderTargets	   = rtvFormats.size();

		for (uint32_t rtvIndex = 0; rtvIndex < rtvFormats.size(); rtvIndex++) { pipelineDesc.RTVFormats[rtvIndex] = rtvFormats.at(rtvIndex); }

		DXGI_FORMAT depthFormat						 = D3D12::GetD3D12PixelFormat(m_Description.DepthFormat);
		pipelineDesc.DSVFormat						 = depthFormat;
		pipelineDesc.BlendState						 = D3D12::CreateBlendStateDesc(m_Description.ColourBlendStates);
		pipelineDesc.DepthStencilState				 = D3D12::CreateDepthStencilDesc(m_Description.DepthStencilDesc);
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

		std::wstring debugName = {m_Description.DebugName.begin(), m_Description.DebugName.end()};
		m_PipelineStateObject->SetName(debugName.c_str());
	}

	ComputePipelineD3D12::ComputePipelineD3D12(ID3D12Device9 *device, const ComputePipelineDescription &description) : ComputePipeline(description)
	{
		const auto &resources = GetRequiredShaderResources();
		D3D12::CreateRootSignature(resources, device, m_RootSignatureBlob, m_RootSignature);
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

		std::wstring debugName = {m_Description.DebugName.begin(), m_Description.DebugName.end()};
		m_PipelineStateObject->SetName(debugName.c_str());
	}
}	 // namespace Nexus::Graphics

#endif