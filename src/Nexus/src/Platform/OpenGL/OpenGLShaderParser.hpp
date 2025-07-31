#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::OpenGL
{
	struct ShaderResource
	{
		std::string Qualififer = {};
		std::string Type	   = {};
		std::string Name	   = {};
	};

	class OpenGLShaderParser
	{
	  public:
		OpenGLShaderParser();
		std::vector<ShaderResource> ReflectShader(const std::string &source);
	};
}	 // namespace Nexus::OpenGL