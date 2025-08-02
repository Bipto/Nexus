#pragma once

#include "Nexus-Core/nxpch.hpp"
#include "ResourceSet.hpp"
#include "ShaderDataType.hpp"

namespace Nexus::Graphics
{

	struct Attribute
	{
		std::string	   Name	   = {};
		ShaderDataType Type	   = {};
		uint32_t	   Binding = {};
	};

	struct ReflectedBufferMember
	{
		std::string			   Name		 = {};
		size_t				   Offset	 = 0;
		size_t				   Size		 = 0;
		std::optional<int32_t> ArraySize = {};
		ShaderDataType		   Type		 = {};
	};

	struct ReflectedUniformBuffer
	{
		std::string						   Name	   = {};
		std::vector<ReflectedBufferMember> Members = {};
	};

	struct ReflectedStorageBuffer
	{
		std::string						   Name	   = {};
		std::vector<ReflectedBufferMember> Members = {};
	};

	struct ReflectedSampler
	{
		std::string Name = {};
	};

	enum class ImageType
	{
		Storage,
		Sampled
	};

	struct ReflectedImages
	{
		std::string Name = {};
		ImageType	Type = ImageType::Sampled;
	};

	struct ShaderReflectionData
	{
		std::vector<Attribute>				Inputs;
		std::vector<Attribute>				Outputs;
		std::vector<ReflectedUniformBuffer> UniformBuffers = {};
		std::vector<ReflectedStorageBuffer> StorageBuffers = {};
		std::vector<ReflectedSampler>		Samplers	   = {};
		std::vector<ReflectedImages>		Images		   = {};

		ShaderReflectionData()							   = default;
		ShaderReflectionData(const ShaderReflectionData &) = default;
		ShaderReflectionData(ShaderReflectionData &&)	   = default;
		~ShaderReflectionData()							   = default;
	};

	enum class ShaderStage
	{
		Invalid = 0,
		Compute,
		Fragment,
		Geometry,
		TesselationControl,
		TesselationEvaluation,
		Vertex,
		RayGeneration,
		RayMiss,
		RayClosestHit,
		RayAnyHit,
		RayIntersection,
		Mesh,
		Task
	};

	struct ShaderAttribute
	{
		std::string	   Name;
		ShaderDataType DataType;
	};

	struct ShaderModuleSpecification
	{
		std::string			  Name = "ShaderModule";
		std::string			  Source;
		ShaderStage			  ShadingStage = ShaderStage::Invalid;
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
			return m_ModuleSpecification.ShadingStage;
		}

		const ShaderModuleSpecification &GetModuleSpecification() const
		{
			return m_ModuleSpecification;
		}

		const ResourceSetSpecification &GetResourceSetSpecification() const
		{
			return m_ResourceSetSpecification;
		}

		virtual ShaderReflectionData Reflect() const = 0;

	  protected:
		ShaderModuleSpecification m_ModuleSpecification;
		ResourceSetSpecification  m_ResourceSetSpecification;
	};
}	 // namespace Nexus::Graphics