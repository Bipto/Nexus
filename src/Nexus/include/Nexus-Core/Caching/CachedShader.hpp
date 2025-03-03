#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/FileSystem/FileSystem.hpp"
#include "Nexus-Core/Graphics/ResourceSet.hpp"
#include "Nexus-Core/Graphics/ShaderModule.hpp"
#include "Nexus-Core/Types.hpp"

#include "yaml-cpp/yaml.h"

namespace YAML
{
	template<>
	struct convert<Nexus::Graphics::ResourceBinding>
	{
		static Node encode(const Nexus::Graphics::ResourceBinding &rhs)
		{
			Node node;
			node.push_back(rhs.Name);
			node.push_back(rhs.Set);
			node.push_back(rhs.Binding);
			node.push_back((uint32_t)rhs.Type);
			return node;
		}

		static bool decode(const Node &node, Nexus::Graphics::ResourceBinding &rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
			{
				return false;
			}

			rhs.Name	= node[0].as<std::string>();
			rhs.Set		= node[1].as<uint32_t>();
			rhs.Binding = node[2].as<uint32_t>();
			rhs.Type	= (Nexus::Graphics::ResourceType)node[3].as<uint32_t>();
			return true;
		}
	};
}	 // namespace YAML

namespace Nexus::Graphics
{
	class NX_API CachedShader
	{
	  public:
		static CachedShader FromModule(const ShaderModuleSpecification &shaderSpec, const ResourceSetSpecification &resourceSpec, size_t hash)
		{
			return CachedShader(shaderSpec, resourceSpec, hash);
		}

		static CachedShader LoadFromFile(const std::string &path)
		{
			return CachedShader(path);
		}

		void Cache(const std::string &path)
		{
			YAML::Node container;

			YAML::Node shaderYAML;
			shaderYAML["Hash"]	 = m_Hash;
			shaderYAML["Name"]	 = m_ShaderSpec.Name;
			shaderYAML["Source"] = m_ShaderSpec.Source;
			shaderYAML["Stage"]	 = (uint32_t)m_ShaderSpec.ShadingStage;
			shaderYAML["SPIRV"]	 = m_ShaderSpec.SpirvBinary;
			container["Shader"]	 = shaderYAML;

			YAML::Node resourceYAML;
			resourceYAML["SampledImages"]  = m_ResourceSpec.SampledImages;
			resourceYAML["UniformBuffers"] = m_ResourceSpec.UniformBuffers;
			container["Resources"]		   = resourceYAML;

			YAML::Emitter out;
			out << container;

			FileSystem::WriteFileAbsolute(path, out.c_str());
		}

		const ShaderModuleSpecification &GetShaderSpecification() const
		{
			return m_ShaderSpec;
		}

		const ResourceSetSpecification &GetResourceSpecification() const
		{
			return m_ResourceSpec;
		}

		size_t GetHash() const
		{
			return m_Hash;
		}

		bool Validate(size_t hash)
		{
			return hash == m_Hash;
		}

	  private:
		CachedShader(const ShaderModuleSpecification &shaderSpec, const ResourceSetSpecification &resourceSpec, size_t hash)
			: m_ShaderSpec(shaderSpec),
			  m_ResourceSpec(resourceSpec),
			  m_Hash(hash)
		{
		}

		CachedShader(const std::string &path)
		{
			const std::string &input = FileSystem::ReadFileToStringAbsolute(path);

			YAML::Node node = YAML::Load(input);

			YAML::Node			  shaderNode = node["Shader"];
			size_t				  hash		 = shaderNode["Hash"].as<size_t>();
			std::string			  name		 = shaderNode["Name"].as<std::string>();
			std::string			  source	 = shaderNode["Source"].as<std::string>();
			ShaderStage			  stage		 = (ShaderStage)shaderNode["Stage"].as<uint32_t>();
			std::vector<uint32_t> spirv		 = shaderNode["SPIRV"].as<std::vector<uint32_t>>();

			YAML::Node					 resourcesNode	= node["Resources"];
			std::vector<ResourceBinding> sampledImages	= resourcesNode["SampledImages"].as<std::vector<ResourceBinding>>();
			std::vector<ResourceBinding> uniformBuffers = resourcesNode["UniformBuffers"].as<std::vector<ResourceBinding>>();

			m_Hash = hash;

			m_ShaderSpec.Name		  = name;
			m_ShaderSpec.Source		  = source;
			m_ShaderSpec.ShadingStage = stage;
			m_ShaderSpec.SpirvBinary  = spirv;

			m_ResourceSpec.SampledImages  = sampledImages;
			m_ResourceSpec.UniformBuffers = uniformBuffers;
		}

	  private:
		ShaderModuleSpecification m_ShaderSpec	 = {};
		ResourceSetSpecification  m_ResourceSpec = {};
		size_t					  m_Hash		 = {};
	};
}	 // namespace Nexus::Graphics