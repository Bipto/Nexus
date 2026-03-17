#pragma once

#include "RHI/RHI-Core.hpp"
#include "RHI/ResourceSet.hpp"
#include "RHI/ShaderDataType.hpp"
#include "RHI/ShaderReflectionData.hpp"
#include "RHI/ShaderResources.hpp"

namespace Nexus::Graphics
{
	struct ShaderModuleSpecification
	{
		std::string			  DebugName = "ShaderModule";
		std::string			  Source;
		ShaderStage			  ShadingStage = ShaderStage::Invalid;
		std::vector<uint32_t> SpirvBinary;

		std::vector<ShaderAttribute> InputAttributes;
		std::vector<ShaderAttribute> OutputAttributes;
	};

	class NX_RHI_API IShaderModule
	{
	  public:
		IShaderModule(const ShaderModuleSpecification &shaderModuleSpec) : m_ModuleSpecification(shaderModuleSpec)
		{
		}

		virtual ~IShaderModule() = default;

		ShaderStage GetShaderStage() const
		{
			return m_ModuleSpecification.ShadingStage;
		}

		const ShaderModuleSpecification &GetModuleSpecification() const
		{
			return m_ModuleSpecification;
		}

		virtual ShaderReflectionData Reflect() const = 0;

	  protected:
		ShaderModuleSpecification m_ModuleSpecification;
	};
}	 // namespace Nexus::Graphics