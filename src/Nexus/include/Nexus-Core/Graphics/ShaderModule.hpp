#pragma once

#include "Nexus-Core/nxpch.hpp"
#include "ResourceSet.hpp"
#include "ShaderDataType.hpp"
#include "ShaderResources.hpp"

namespace Nexus::Graphics
{
	struct ReflectedResource
	{
		ReflectedShaderDataType Type				  = ReflectedShaderDataType::UniformBuffer;
		std::string				Name				  = {};
		ResourceDimension		Dimension			  = ResourceDimension::None;
		StorageResourceAccess	StorageResourceAccess = StorageResourceAccess::None;
		uint32_t				DescriptorSet		  = 0;
		uint32_t				BindingPoint		  = 0;
		uint32_t				BindingCount		  = 0;
		uint32_t				RegisterSpace		  = 0;
	};

	struct Attribute
	{
		std::string				Name		= {};
		ReflectedShaderDataType Type		= {};
		uint32_t				Binding		= {};
		uint32_t				StreamIndex = {};
	};

	struct ReflectedBufferMember
	{
		std::string				Name	  = {};
		size_t					Offset	  = 0;
		size_t					Size	  = 0;
		std::optional<uint32_t> ArraySize = {};
		ShaderDataType			Type	  = {};
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

	struct ShaderReflectionData
	{
		std::vector<Attribute>				Inputs;
		std::vector<Attribute>				Outputs;
		std::vector<ReflectedUniformBuffer> UniformBuffers = {};
		std::vector<ReflectedStorageBuffer> StorageBuffers = {};
		std::vector<ReflectedResource>		Resources	   = {};
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
		ShaderModule(const ShaderModuleSpecification &shaderModuleSpec) : m_ModuleSpecification(shaderModuleSpec)
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

		virtual ShaderReflectionData Reflect() const = 0;

	  protected:
		ShaderModuleSpecification m_ModuleSpecification;
	};
}	 // namespace Nexus::Graphics