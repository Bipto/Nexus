#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/FileSystem/FileSystem.hpp"
#include "Nexus-Core/Graphics/ResourceSet.hpp"
#include "Nexus-Core/Graphics/ShaderModule.hpp"
#include "Nexus-Core/Types.hpp"

#include "yaml-cpp/yaml.h"

namespace Nexus::Graphics
{
	class NX_API CachedShader
	{
	  public:
		static CachedShader FromModule(const ShaderModuleSpecification &shaderSpec, size_t hash)
		{
			return CachedShader(shaderSpec, hash);
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

			YAML::Emitter out;
			out << container;

			FileSystem::WriteFileAbsolute(path, out.c_str());
		}

		const ShaderModuleSpecification &GetShaderSpecification() const
		{
			return m_ShaderSpec;
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
		CachedShader(const ShaderModuleSpecification &shaderSpec, size_t hash) : m_ShaderSpec(shaderSpec), m_Hash(hash)
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
			;

			m_Hash = hash;

			m_ShaderSpec.Name		  = name;
			m_ShaderSpec.Source		  = source;
			m_ShaderSpec.ShadingStage = stage;
			m_ShaderSpec.SpirvBinary  = spirv;
		}

	  private:
		ShaderModuleSpecification m_ShaderSpec	 = {};
		size_t					  m_Hash		 = {};
	};
}	 // namespace Nexus::Graphics