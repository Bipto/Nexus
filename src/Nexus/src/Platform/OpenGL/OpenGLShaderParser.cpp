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

		std::string inputSource = source;

		// Strip comments and normalize whitespace
		inputSource = std::regex_replace(inputSource, std::regex(R"(//.*?$|/\*.*?\*/)", std::regex::ECMAScript), "");
		inputSource = std::regex_replace(inputSource, std::regex(R"([\r\n]+)"), "\n");	  // collapse line breaks
		inputSource = std::regex_replace(inputSource, std::regex(R"(\s{2,})"), " ");	  // collapse excessive spaces

		// reflect custom structs
		{
			std::regex pattern(R"(struct\s+(\w+)\s*\{([\s\S]*?)\}\s*;)");

			auto begin = std::sregex_iterator(inputSource.begin(), inputSource.end(), pattern);
			auto end   = std::sregex_iterator();

			for (auto i = begin; i != end; ++i)
			{
				std::smatch match = *i;

				ReflectedCustomStructs &customStruct = reflectionData.CustomStructs.emplace_back();
				customStruct.Name					 = match[1];

				std::string body = match[2];

				std::regex memberRegex(R"(\s*(\w+)\s+(\w+)((?:\s*\[\s*\d*\s*\])*)\s*;)");
				auto	   memberBegin = std::sregex_iterator(body.begin(), body.end(), memberRegex);
				auto	   memberEnd   = std::sregex_iterator();

				for (auto it = memberBegin; it != memberEnd; ++it)
				{
					std::smatch memberMatch = *it;

					ReflectedBufferMember &member = customStruct.Members.emplace_back();
					member.Type					  = memberMatch[1];
					member.Name					  = memberMatch[2];
					member.ArraySuffix			  = memberMatch[3];
				}
			}
		}

		// reflect attributes
		{
			std::regex pattern(R"((?:layout\s*\(\s*([^)]+)\s*\)\s*)?(uniform|in|out)\s+(\w+)\s+(\w+)((?:\s*\[\s*\d*\s*\])*)\s*;)",
							   std::regex::ECMAScript);

			auto begin = std::sregex_iterator(inputSource.begin(), inputSource.end(), pattern);
			auto end   = std::sregex_iterator();

			for (auto i = begin; i != end; ++i)
			{
				std::smatch match = *i;

				ReflectedShaderUniforms &resource = reflectionData.Uniforms.emplace_back();
				resource.LayoutQualififers		  = match[1];
				resource.StorageQualifier		  = match[2];
				resource.Type					  = match[3];
				resource.Name					  = match[4];
				resource.ArraySuffix			  = match[5];
			}
		}

		// reflect buffer blocks
		{
			std::regex pattern(R"((?:layout\s*\(\s*([^)]+)\s*\)\s*)?(uniform|buffer|shared)\s+(\w+)\s*\{([\s\S]*?)\}\s*(\w+)?\s*;)",
							   std::regex::ECMAScript);

			auto begin = std::sregex_iterator(inputSource.begin(), inputSource.end(), pattern);
			auto end   = std::sregex_iterator();

			for (auto i = begin; i != end; ++i)
			{
				std::smatch match = *i;

				ReflectedShaderBuffer &buffer = reflectionData.Buffers.emplace_back();
				buffer.LayoutQualifiers		  = match[1];
				buffer.StorageQualifier		  = match[2];
				buffer.BlockName			  = match[3];
				buffer.InstanceName			  = match[5].matched ? match[5].str() : std::string {};

				// extract members
				{
					std::string blockContents = match[4];
					std::regex	memberPattern(R"(\s*(\w+)\s+(\w+)((?:\s*\[\s*\d*\s*\])*)\s*;)");

					auto memberBegin = std::sregex_iterator(blockContents.begin(), blockContents.end(), memberPattern);
					auto memberEnd	 = std::sregex_iterator();

					for (auto it = memberBegin; it != memberEnd; ++it)
					{
						std::smatch memberMatch = *it;

						ReflectedBufferMember &member = buffer.Members.emplace_back();
						member.Type		   = memberMatch[1];
						member.Name		   = memberMatch[2];
						member.ArraySuffix			  = memberMatch[3];
					}
				}
			}
		}

		return reflectionData;
	}
}	 // namespace Nexus::OpenGL
