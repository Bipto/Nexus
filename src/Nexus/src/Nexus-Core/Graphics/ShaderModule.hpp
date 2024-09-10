#pragma once

#include "Nexus-Core/nxpch.hpp"
#include "ResourceSet.hpp"
#include "ShaderDataType.hpp"

namespace Nexus::Graphics
{
	enum class ShaderStage
	{
		None = 0,
		Compute,
		Fragment,
		Geometry,
		TesselationControl,
		TesselationEvaluation,
		Vertex
	};

	struct ShaderAttribute
	{
		std::string	   Name;
		ShaderDataType Type;
	};

	struct ShaderModuleSpecification
	{
		std::string			  Name = "ShaderModule";
		std::string			  Source;
		ShaderStage			  Stage = ShaderStage::None;
		std::vector<uint32_t> SpirvBinary;

		std::vector<ShaderAttribute> InputAttributes;
		std::vector<ShaderAttribute> OutputAttributes;
	};

	class ShaderModule
	{
	  public:
		ShaderModule(const ShaderModuleSpecification &shaderModuleSpec, const ResourceSetSpecification &resourceSpec)
			: m_ModuleSpecification(shaderModuleSpec),
			  m_ResourceSetSpecification(resourceSpec)
		{
		}

		virtual ~ShaderModule() = default;

		ShaderStage GetShaderStage() const
		{
			return m_ModuleSpecification.Stage;
		}
		const ShaderModuleSpecification &GetModuleSpecification() const
		{
			return m_ModuleSpecification;
		}
		const ResourceSetSpecification &GetResourceSetSpecification() const
		{
			return m_ResourceSetSpecification;
		}

	  protected:
		ShaderModuleSpecification m_ModuleSpecification;
		ResourceSetSpecification  m_ResourceSetSpecification;
	};
}	 // namespace Nexus::Graphics