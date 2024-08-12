#if defined(NX_PLATFORM_D3D12)

#include "SamplerD3D12.hpp"

#include "D3D12Utils.hpp"

namespace Nexus::Graphics
{
    SamplerD3D12::SamplerD3D12(const SamplerSpecification &spec)
        : m_Specification(spec)
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

    const SamplerSpecification &SamplerD3D12::GetSamplerSpecification()
    {
        return m_Specification;
    }

    D3D12_FILTER SamplerD3D12::GetFilter()
    {
        return m_SamplerFilter;
    }

    D3D12_TEXTURE_ADDRESS_MODE SamplerD3D12::GetAddressModeU()
    {
        return m_AddressModeU;
    }

    D3D12_TEXTURE_ADDRESS_MODE SamplerD3D12::GetAddressModeV()
    {
        return m_AddressModeV;
    }

    D3D12_TEXTURE_ADDRESS_MODE SamplerD3D12::GetAddressModeW()
    {
        return m_AddressModeW;
    }

    D3D12_COMPARISON_FUNC SamplerD3D12::GetComparisonFunc()
    {
        return m_ComparisonFunc;
    }
}

#endif