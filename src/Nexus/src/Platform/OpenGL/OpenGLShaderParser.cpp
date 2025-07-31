#include "OpenGLShaderParser.hpp"

#include <regex>

namespace Nexus::OpenGL
{
	OpenGLShaderParser::OpenGLShaderParser()
	{
	}

	std::vector<ShaderResource> OpenGLShaderParser::ReflectShader(const std::string &source)
	{
		std::vector<ShaderResource> resources;
		std::regex					pattern(R"((uniform|attribute|varying|in|out)\s+(\w+)\s+(\w+)\s*;)");

		auto begin = std::sregex_iterator(source.begin(), source.end(), pattern);
		auto end   = std::sregex_iterator();

		for (auto i = begin; i != end; ++i)
		{
			std::smatch match = *i;
			resources.push_back({match[1], match[2], match[3]});
		}

		return resources;
	}
}	 // namespace Nexus::OpenGL
