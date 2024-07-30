#pragma once

#if defined(NX_PLATFORM_D3D12)

#include "D3D12Include.hpp"
#include "Nexus-Core/Graphics/Pipeline.hpp"

namespace Nexus::Graphics
{

    class PipelineD3D12 : public Pipeline
    {
    public:
        PipelineD3D12(ID3D12Device10 *device, const PipelineDescription &description);
        virtual ~PipelineD3D12();
        virtual const PipelineDescription &GetPipelineDescription() const override;
        ID3D12RootSignature *GetRootSignature();
        ID3D12PipelineState *GetPipelineState();
        D3D_PRIMITIVE_TOPOLOGY GetD3DPrimitiveTopology();

    private:
        void CreateRootSignature();
        void CreateInputLayout();
        void CreatePrimitiveTopology();

        D3D12_RASTERIZER_DESC CreateRasterizerState();
        D3D12_STREAM_OUTPUT_DESC CreateStreamOutputDesc();
        D3D12_BLEND_DESC CreateBlendStateDesc();
        D3D12_DEPTH_STENCIL_DESC CreateDepthStencilDesc();
        D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPipelinePrimitiveTopologyType();

        void CreatePipeline();

    private:
        PipelineDescription m_Description;
        ID3D12Device10 *m_Device;

        Microsoft::WRL::ComPtr<ID3DBlob> m_RootSignatureBlob;
        Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;

        std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;

        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineStateObject = nullptr;

        D3D_PRIMITIVE_TOPOLOGY m_PrimitiveTopology;
    };
}

#endif