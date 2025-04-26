#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Include.hpp"
	#include "Nexus-Core/Graphics/Pipeline.hpp"

namespace Nexus::Graphics
{

	class GraphicsPipelineD3D12 : public GraphicsPipeline
	{
	  public:
		GraphicsPipelineD3D12(ID3D12Device9 *device, const GraphicsPipelineDescription &description);
		virtual ~GraphicsPipelineD3D12();
		virtual const GraphicsPipelineDescription &GetPipelineDescription() const override;
		ID3D12RootSignature				  *GetRootSignature();
		ID3D12PipelineState				  *GetPipelineState();
		D3D_PRIMITIVE_TOPOLOGY			   GetD3DPrimitiveTopology();

	  private:
		void CreateRootSignature();
		void CreateInputLayout();
		void CreatePrimitiveTopology();

		D3D12_RASTERIZER_DESC	 CreateRasterizerState();
		D3D12_STREAM_OUTPUT_DESC CreateStreamOutputDesc();
		D3D12_BLEND_DESC		 CreateBlendStateDesc();
		D3D12_DEPTH_STENCIL_DESC CreateDepthStencilDesc();

		void CreatePipeline();

	  private:
		GraphicsPipelineDescription m_Description;
		ID3D12Device9	   *m_Device;

		Microsoft::WRL::ComPtr<ID3DBlob>			m_RootSignatureBlob;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;

		std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineStateObject = nullptr;

		D3D_PRIMITIVE_TOPOLOGY m_PrimitiveTopology;
	};
}	 // namespace Nexus::Graphics

#endif