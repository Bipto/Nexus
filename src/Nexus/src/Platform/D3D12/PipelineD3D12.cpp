#include "PipelineD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Utils.hpp"
	#include "FramebufferD3D12.hpp"
	#include "ShaderModuleD3D12.hpp"
	#include "SwapchainD3D12.hpp"

namespace Nexus::Graphics
{

	GraphicsPipelineD3D12::GraphicsPipelineD3D12(GraphicsDeviceD3D12 *device, const GraphicsPipelineDescription &description)
		: GraphicsPipeline(description),
		  m_Description(description)
	{
		const auto &resources = GetRequiredShaderResources();
		D3D12::CreateRootSignature(resources, device->GetDevice(), m_RootSignatureBlob, m_RootSignature, m_DescriptorHandleInfo);
		m_InputLayout = D3D12::CreateInputLayout(description.Layouts);
		m_PipelineStateObject = D3D12::CreateGraphicsPipeline(device, description, m_RootSignature, m_InputLayout);
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

	const D3D12::DescriptorHandleInfo &GraphicsPipelineD3D12::GetDescriptorHandleInfo()
	{
		return m_DescriptorHandleInfo;
	}

	ComputePipelineD3D12::ComputePipelineD3D12(GraphicsDeviceD3D12 *device, const ComputePipelineDescription &description)
		: ComputePipeline(description)
	{
		const auto &resources = GetRequiredShaderResources();
		D3D12::CreateRootSignature(resources, device->GetDevice(), m_RootSignatureBlob, m_RootSignature, m_DescriptorHandleInfo);
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

	const D3D12::DescriptorHandleInfo &ComputePipelineD3D12::GetDescriptorHandleInfo()
	{
		return m_DescriptorHandleInfo;
	}

	void ComputePipelineD3D12::CreatePipeline(GraphicsDeviceD3D12 *device)
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

		auto	d3d12Device = device->GetDevice();
		HRESULT hr			= d3d12Device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&m_PipelineStateObject));
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