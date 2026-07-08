#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Platform/FileSystem/FileSystem.hpp"
#include "RHI/ResourceSet.hpp"
#include "RHI/ShaderModule.hpp"
#include "RHI/Types.hpp"

#include "yaml-cpp/yaml.h"

namespace Nexus::Graphics
{
    class NX_API CachedShader
    {
      public:
        static CachedShader FromModule(const ShaderModuleDescription &shaderSpec, size_t hash)
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
            shaderYAML["Hash"] = m_Hash;
            shaderYAML["Name"] = m_ShaderModuleDesc.DebugName;
            shaderYAML["Source"] = m_ShaderModuleDesc.Source;
            shaderYAML["Stage"] = (uint32_t)m_ShaderModuleDesc.ShadingStage;
            shaderYAML["SPIRV"] = m_ShaderModuleDesc.SpirvBinary;
            container["Shader"] = shaderYAML;

            YAML::Emitter out;
            out << container;

            FileSystem::WriteFileAbsolute(path, out.c_str());
        }

        const ShaderModuleDescription &GetShaderModuleDescription() const
        {
            return m_ShaderModuleDesc;
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
        CachedShader(const ShaderModuleDescription &shaderSpec, size_t hash)
            : m_ShaderModuleDesc(shaderSpec), m_Hash(hash)
        {
        }

        CachedShader(const std::string &path)
        {
            const std::string &input = FileSystem::ReadFileToStringAbsolute(path);

            YAML::Node node = YAML::Load(input);

            YAML::Node shaderNode = node["Shader"];
            size_t hash = shaderNode["Hash"].as<size_t>();
            std::string name = shaderNode["Name"].as<std::string>();
            std::string source = shaderNode["Source"].as<std::string>();
            ShaderStage stage = (ShaderStage)shaderNode["Stage"].as<uint32_t>();
            std::vector<uint32_t> spirv = shaderNode["SPIRV"].as<std::vector<uint32_t>>();
            ;

            m_Hash = hash;

            m_ShaderModuleDesc.DebugName = name;
            m_ShaderModuleDesc.Source = source;
            m_ShaderModuleDesc.ShadingStage = stage;
            m_ShaderModuleDesc.SpirvBinary = spirv;
        }

      private:
        ShaderModuleDescription m_ShaderModuleDesc = {};
        size_t m_Hash = {};
    };
} // namespace Nexus::Graphics