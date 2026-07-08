#pragma once

#if defined(NX_PLATFORM_D3D12)

#include "D3D12Include.hpp"
#include "D3D12Utils.hpp"
#include "RHI/Pipeline.hpp"

namespace Nexus::Graphics
{

    class PipelineD3D12
    {
      public:
        virtual ~PipelineD3D12()
        {
        }
        virtual void Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList) = 0;
        virtual const D3D12::DescriptorHandleInfo &GetDescriptorHandleInfo() const = 0;
        virtual const D3D12::RootSignatureBindingLocations &GetRootSignatureBindingLocations() const = 0;
    };

    class GraphicsPipelineD3D12 : public IGraphicsPipeline, public PipelineD3D12
    {
      public:
        GraphicsPipelineD3D12(GraphicsDeviceD3D12 *device, const GraphicsPipelineDescription &description);
        virtual ~GraphicsPipelineD3D12();
        virtual const GraphicsPipelineDescription &GetPipelineDescription() const override;
        Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() const;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineState() const;
        D3D_PRIMITIVE_TOPOLOGY GetD3DPrimitiveTopology() const;
        const D3D12::RootSignatureBindingLocations &GetRootSignatureBindingLocations() const final;

        void Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList) final;
        const D3D12::DescriptorHandleInfo &GetDescriptorHandleInfo() const final;

      private:
        GraphicsPipelineDescription m_Description;

        Microsoft::WRL::ComPtr<ID3DBlob> m_RootSignatureBlob;
        Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
        D3D12::DescriptorHandleInfo m_DescriptorHandleInfo = {};

        std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineStateObject = nullptr;
        D3D_PRIMITIVE_TOPOLOGY m_PrimitiveTopology;

        D3D12::RootSignatureBindingLocations m_RootSignatureBindingLocations = {};
    };

    class MeshletPipelineD3D12 : public IMeshletPipeline, public PipelineD3D12
    {
      public:
        MeshletPipelineD3D12(GraphicsDeviceD3D12 *device, const MeshletPipelineDescription &description);
        virtual ~MeshletPipelineD3D12();
        Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() const;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineState() const;
        D3D_PRIMITIVE_TOPOLOGY GetD3DPrimitiveTopology() const;
        const D3D12::RootSignatureBindingLocations &GetRootSignatureBindingLocations() const final;

        void Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList) final;
        const D3D12::DescriptorHandleInfo &GetDescriptorHandleInfo() const final;

      private:
        GraphicsPipelineDescription m_Description;

        Microsoft::WRL::ComPtr<ID3DBlob> m_RootSignatureBlob;
        Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
        D3D12::DescriptorHandleInfo m_DescriptorHandleInfo = {};

        std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineStateObject = nullptr;
        D3D_PRIMITIVE_TOPOLOGY m_PrimitiveTopology;

        D3D12::RootSignatureBindingLocations m_RootSignatureBindingLocations = {};
    };

    class ComputePipelineD3D12 : public IComputePipeline, public PipelineD3D12
    {
      public:
        ComputePipelineD3D12(GraphicsDeviceD3D12 *device, const ComputePipelineDescription &description);
        virtual ~ComputePipelineD3D12();
        void Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList) final;
        const D3D12::DescriptorHandleInfo &GetDescriptorHandleInfo() const final;
        const D3D12::RootSignatureBindingLocations &GetRootSignatureBindingLocations() const final;

      private:
        Microsoft::WRL::ComPtr<ID3DBlob> m_RootSignatureBlob;
        Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineStateObject = nullptr;
        D3D12::DescriptorHandleInfo m_DescriptorHandleInfo = {};

        D3D12::RootSignatureBindingLocations m_RootSignatureBindingLocations = {};
    };
} // namespace Nexus::Graphics

#endif