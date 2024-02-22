#pragma once

#include "D3D12Include.hpp"
#include "Nexus/Graphics/Sampler.hpp"

namespace Nexus::Graphics
{
    class SamplerD3D12 : public Sampler
    {
    public:
        SamplerD3D12(const SamplerSpecification &spec);
        virtual ~SamplerD3D12();
        virtual const SamplerSpecification &GetSamplerSpecification() override;

        D3D12_FILTER GetFilter();
        D3D12_TEXTURE_ADDRESS_MODE GetAddressModeU();
        D3D12_TEXTURE_ADDRESS_MODE GetAddressModeV();
        D3D12_TEXTURE_ADDRESS_MODE GetAddressModeW();
        D3D12_COMPARISON_FUNC GetComparisonFunc();

    private:
        SamplerSpecification m_Specification;
        D3D12_FILTER m_SamplerFilter;
        D3D12_TEXTURE_ADDRESS_MODE m_AddressModeU;
        D3D12_TEXTURE_ADDRESS_MODE m_AddressModeV;
        D3D12_TEXTURE_ADDRESS_MODE m_AddressModeW;
        D3D12_COMPARISON_FUNC m_ComparisonFunc;
    };
}