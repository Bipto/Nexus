#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Include.hpp"
	#include "Nexus-Core/Graphics/ShaderModule.hpp"

namespace Nexus::Graphics
{
	class ShaderModuleD3D12 : public ShaderModule
	{
	  public:
		ShaderModuleD3D12(const ShaderModuleSpecification &shaderModuleSpec, const ResourceSetSpecification &resourceSpec);
		Microsoft::WRL::ComPtr<IDxcBlob> GetBlob() const;

	  private:
		Microsoft::WRL::ComPtr<IDxcBlob> m_ShaderBlob = nullptr;
	};
}	 // namespace Nexus::Graphics

#endif