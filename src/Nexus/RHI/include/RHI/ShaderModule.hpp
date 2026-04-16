#pragma once

#include "Core/ResourcePool.hpp"

#include "RHI/RHI-Core.hpp"
#include "RHI/ResourceSet.hpp"
#include "RHI/ShaderDataType.hpp"
#include "RHI/ShaderReflectionData.hpp"
#include "RHI/ShaderResources.hpp"

namespace Nexus::Graphics
{
	struct ShaderModuleDescription
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
		IShaderModule(const ShaderModuleDescription &shaderModuleDesc) : m_ModuleDescription(shaderModuleDesc)
		{
		}

		virtual ~IShaderModule() = default;

		ShaderStage GetShaderStage() const
		{
			return m_ModuleDescription.ShadingStage;
		}

		const ShaderModuleDescription &GetModuleDescription() const
		{
			return m_ModuleDescription;
		}

		virtual ShaderReflectionData Reflect() const = 0;

	  protected:
		ShaderModuleDescription m_ModuleDescription;
	};

	DEFINE_RESOURCE(ShaderModule, IShaderModule);
}	 // namespace Nexus::Graphics