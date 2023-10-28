#pragma once

#include "D3D12Include.hpp"
#include "Nexus/Graphics/Pipeline.hpp"

namespace Nexus::Graphics
{

    class PipelineD3D12 : public Pipeline
    {
    public:
        PipelineD3D12(ID3D12Device10 *device, const PipelineDescription &description);
        virtual ~PipelineD3D12();
        virtual const PipelineDescription &GetPipelineDescription() const override;

    private:
        PipelineDescription m_Description;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineStateObject = nullptr;
    };
}