#if defined(NX_PLATFORM_D3D12)

#include "SamplerD3D12.hpp"

#include "D3D12Utils.hpp"

namespace Nexus::Graphics
{
    SamplerD3D12::SamplerD3D12(const SamplerDescription &spec) : m_Description(spec)
    {
        m_SamplerFilter = D3D12::GetD3D12Filter(spec.SampleFilter);
        m_AddressModeU = D3D12::GetD3D12TextureAddressMode(spec.AddressModeU);
        m_AddressModeV = D3D12::GetD3D12TextureAddressMode(spec.AddressModeV);
        m_AddressModeW = D3D12::GetD3D12TextureAddressMode(spec.AddressModeW);
        m_ComparisonFunc = D3D12::GetComparisonFunction(spec.SamplerComparisonFunction);
    }

    SamplerD3D12::~SamplerD3D12()
    {
    }

    const SamplerDescription &SamplerD3D12::GetSamplerDescription() const
    {
        return m_Description;
    }

    const D3D12_FILTER SamplerD3D12::GetFilter() const
    {
        return m_SamplerFilter;
    }

    const D3D12_TEXTURE_ADDRESS_MODE SamplerD3D12::GetAddressModeU() const
    {
        return m_AddressModeU;
    }

    const D3D12_TEXTURE_ADDRESS_MODE SamplerD3D12::GetAddressModeV() const
    {
        return m_AddressModeV;
    }

    const D3D12_TEXTURE_ADDRESS_MODE SamplerD3D12::GetAddressModeW() const
    {
        return m_AddressModeW;
    }

    const D3D12_COMPARISON_FUNC SamplerD3D12::GetComparisonFunc() const
    {
        return m_ComparisonFunc;
    }
} // namespace Nexus::Graphics

#endif