#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Include.hpp"
	#include "RHI/Sampler.hpp"

namespace Nexus::Graphics
{
	class SamplerD3D12 final : public ISampler
	{
	  public:
		SamplerD3D12(const SamplerDescription &spec);
		virtual ~SamplerD3D12();
		const SamplerDescription &GetSamplerDescription() const final;

		const D3D12_FILTER				 GetFilter() const;
		const D3D12_TEXTURE_ADDRESS_MODE GetAddressModeU() const;
		const D3D12_TEXTURE_ADDRESS_MODE GetAddressModeV() const;
		const D3D12_TEXTURE_ADDRESS_MODE GetAddressModeW() const;
		const D3D12_COMPARISON_FUNC		 GetComparisonFunc() const;

	  private:
		SamplerDescription		   m_Description;
		D3D12_FILTER			   m_SamplerFilter;
		D3D12_TEXTURE_ADDRESS_MODE m_AddressModeU;
		D3D12_TEXTURE_ADDRESS_MODE m_AddressModeV;
		D3D12_TEXTURE_ADDRESS_MODE m_AddressModeW;
		D3D12_COMPARISON_FUNC	   m_ComparisonFunc;
	};
}	 // namespace Nexus::Graphics

#endif