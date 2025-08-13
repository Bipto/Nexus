#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Include.hpp"
	#include "D3D12Utils.hpp"
	#include "Nexus-Core/Graphics/Pipeline.hpp"

namespace Nexus::Graphics
{

	class PipelineD3D12
	{
	  public:
		virtual ~PipelineD3D12()
		{
		}
		virtual void							   Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList) = 0;
		virtual const D3D12::DescriptorHandleInfo &GetDescriptorHandleInfo()											= 0;
	};

	class GraphicsPipelineD3D12 : public GraphicsPipeline, public PipelineD3D12
	{
	  public:
		GraphicsPipelineD3D12(GraphicsDeviceD3D12 *device, const GraphicsPipelineDescription &description);
		virtual ~GraphicsPipelineD3D12();
		virtual const GraphicsPipelineDescription  &GetPipelineDescription() const override;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature();
		Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineState();
		D3D_PRIMITIVE_TOPOLOGY						GetD3DPrimitiveTopology();

		void							   Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList) final;
		const D3D12::DescriptorHandleInfo &GetDescriptorHandleInfo() final;

	  private:
		GraphicsPipelineDescription m_Description;

		Microsoft::WRL::ComPtr<ID3DBlob>			m_RootSignatureBlob;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
		D3D12::DescriptorHandleInfo					m_DescriptorHandleInfo = {};

		std::vector<D3D12_INPUT_ELEMENT_DESC>		m_InputLayout;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineStateObject = nullptr;
		D3D_PRIMITIVE_TOPOLOGY						m_PrimitiveTopology;
	};

	class MeshletPipelineD3D12 : public MeshletPipeline, public PipelineD3D12
	{
	  public:
		MeshletPipelineD3D12(GraphicsDeviceD3D12 *device, const MeshletPipelineDescription &description);
		virtual ~MeshletPipelineD3D12();
		Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature();
		Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineState();
		D3D_PRIMITIVE_TOPOLOGY						GetD3DPrimitiveTopology();

		void							   Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList) final;
		const D3D12::DescriptorHandleInfo &GetDescriptorHandleInfo() final;

	  private:
		GraphicsPipelineDescription m_Description;

		Microsoft::WRL::ComPtr<ID3DBlob>			m_RootSignatureBlob;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
		D3D12::DescriptorHandleInfo					m_DescriptorHandleInfo = {};

		std::vector<D3D12_INPUT_ELEMENT_DESC>		m_InputLayout;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineStateObject = nullptr;
		D3D_PRIMITIVE_TOPOLOGY						m_PrimitiveTopology;
	};

	class ComputePipelineD3D12 : public ComputePipeline, public PipelineD3D12
	{
	  public:
		ComputePipelineD3D12(GraphicsDeviceD3D12 *device, const ComputePipelineDescription &description);
		virtual ~ComputePipelineD3D12();
		void Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList) final;
		const D3D12::DescriptorHandleInfo &GetDescriptorHandleInfo() final;

	  private:
		Microsoft::WRL::ComPtr<ID3DBlob>			m_RootSignatureBlob;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineStateObject  = nullptr;
		D3D12::DescriptorHandleInfo					m_DescriptorHandleInfo = {};
	};
}	 // namespace Nexus::Graphics

#endif