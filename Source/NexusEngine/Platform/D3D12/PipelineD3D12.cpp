#include "PipelineD3D12.hpp"

namespace Nexus::Graphics
{
    PipelineD3D12::PipelineD3D12(ID3D12Device10 *device, const PipelineDescription &description)
        : Pipeline(description)
    {
    }

    PipelineD3D12::~PipelineD3D12()
    {
    }

    const PipelineDescription &PipelineD3D12::GetPipelineDescription() const
    {
        return m_Description;
    }
}