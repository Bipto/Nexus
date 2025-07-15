#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Include.hpp"
	#include "Nexus-Core/Graphics/Pipeline.hpp"

namespace Nexus::Graphics
{

	class PipelineD3D12
	{
	  public:
		virtual ~PipelineD3D12()
		{
		}
		virtual void Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList) = 0;
	};

	class GraphicsPipelineD3D12 : public GraphicsPipeline, public PipelineD3D12
	{
	  public:
		GraphicsPipelineD3D12(ID3D12Device9 *device, const GraphicsPipelineDescription &description);
		virtual ~GraphicsPipelineD3D12();
		virtual const GraphicsPipelineDescription &GetPipelineDescription() const override;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature();
		Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineState();
		D3D_PRIMITIVE_TOPOLOGY					   GetD3DPrimitiveTopology();

		void Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList) final;

	  private:
		void CreatePipeline(ID3D12Device9 *device);

	  private:
		GraphicsPipelineDescription m_Description;

		Microsoft::WRL::ComPtr<ID3DBlob>			m_RootSignatureBlob;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;

		std::vector<D3D12_INPUT_ELEMENT_DESC>		m_InputLayout;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineStateObject = nullptr;
		D3D_PRIMITIVE_TOPOLOGY						m_PrimitiveTopology;
	};

	class ComputePipelineD3D12 : public ComputePipeline, public PipelineD3D12
	{
	  public:
		ComputePipelineD3D12(ID3D12Device9 *device, const ComputePipelineDescription &description);
		virtual ~ComputePipelineD3D12();
		void Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList) final;

	  private:
		void CreatePipeline(ID3D12Device9 *device);

	  private:
		Microsoft::WRL::ComPtr<ID3DBlob>			m_RootSignatureBlob;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineStateObject = nullptr;
	};
}	 // namespace Nexus::Graphics

#endif