#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Include.hpp"
	#include "Nexus-Core/Graphics/ShaderModule.hpp"

namespace Nexus::Graphics
{
	class ShaderModuleD3D12 : public ShaderModule
	{
	  public:
		ShaderModuleD3D12(const ShaderModuleSpecification &shaderModuleSpec);
		Microsoft::WRL::ComPtr<IDxcBlob> GetBlob() const;
		ShaderReflectionData			 Reflect() const final;

	  private:
		void ReflectShader(Microsoft::WRL::ComPtr<IDxcUtils> utils, Microsoft::WRL::ComPtr<IDxcResult> compileResult);

	  private:
		Microsoft::WRL::ComPtr<IDxcBlob> m_ShaderBlob = nullptr;
		ShaderReflectionData			 m_ReflectionData = {};
	};
}	 // namespace Nexus::Graphics

#endif