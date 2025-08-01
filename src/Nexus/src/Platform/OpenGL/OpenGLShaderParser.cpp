#include "OpenGLShaderParser.hpp"

#include <regex>

namespace Nexus::OpenGL
{
	OpenGLShaderParser::OpenGLShaderParser()
	{
	}

	ShaderReflectionData OpenGLShaderParser::ReflectShader(const std::string &source)
	{
		ShaderReflectionData reflectionData;

		// reflect attributes
		{
			std::regex pattern(R"((?:layout\s*\(\s*([^)]+)\s*\)\s*)?(uniform|in|out)\s+(\w+)\s+(\w+)(\s*\[\s*\d*\s*\])?\s*;)",
							   std::regex::ECMAScript);

			auto begin = std::sregex_iterator(source.begin(), source.end(), pattern);
			auto end   = std::sregex_iterator();

			for (auto i = begin; i != end; ++i)
			{
				std::smatch match = *i;

				ReflectedShaderUniforms &resource = reflectionData.Uniforms.emplace_back();
				resource.LayoutQualififer		  = match[1];
				resource.LayoutValue			  = match[2];
				resource.StorageQualifier		  = match[3];
				resource.Type					  = match[4];
				resource.Name					  = match[5];
				resource.ArraySuffix			  = match[6];
			}
		}

		// reflect uniform blocks
		{
			std::regex pattern(R"((?:layout\s*\(\s*([^)]+)\s*\)\s*)?uniform\s+(\w+)\s*\{([\s\S]*?)\}\s*(\w+)?\s*;)", std::regex::ECMAScript);

			auto begin = std::sregex_iterator(source.begin(), source.end(), pattern);
			auto end   = std::sregex_iterator();

			for (auto i = begin; i != end; ++i)
			{
				std::smatch match = *i;

				ReflectedUniformBuffer &buffer = reflectionData.UniformBuffers.emplace_back();
				buffer.LayoutQualifiers		   = match[1];
				buffer.BlockName			   = match[2];

				// extract members
				{
					std::string blockContents = match[3];
					std::regex	memberPattern(R"(\s*(\w+)\s+(\w+)(\s*\[\s*\d*\s*\])?\s*;)");

					auto memberBegin = std::sregex_iterator(blockContents.begin(), blockContents.end(), memberPattern);
					auto memberEnd	 = std::sregex_iterator();

					for (auto it = memberBegin; it != memberEnd; ++it)
					{
						std::smatch memberMatch = *it;

						ReflectedUniformMember member;
						member.Type		   = memberMatch[1];
						member.Name		   = memberMatch[2];
						member.ArraySuffix = memberMatch[3];

						buffer.Members.emplace_back(std::move(member));
					}
				}

				buffer.InstanceName = match[4];
			}
		}

		return reflectionData;
	}
}	 // namespace Nexus::OpenGL
