#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::OpenGL
{

	struct ReflectedShaderUniforms
	{
		std::string LayoutQualififer = {};
		std::string LayoutValue		 = {};
		std::string StorageQualifier = {};
		std::string Type			 = {};
		std::string Name			 = {};
		std::string ArraySuffix		 = {};
	};

	struct ReflectedUniformMember
	{
		std::string Type		= {};
		std::string Name		= {};
		std::string ArraySuffix = {};
	};

	struct ReflectedUniformBuffer
	{
		std::string							LayoutQualifiers = {};
		std::string							BlockName		 = {};
		std::string							InstanceName	 = {};
		std::vector<ReflectedUniformMember> Members			 = {};
	};

	struct ShaderReflectionData
	{
		std::vector<ReflectedShaderUniforms> Uniforms		= {};
		std::vector<ReflectedUniformBuffer>	 UniformBuffers = {};
	};

	class OpenGLShaderParser
	{
	  public:
		OpenGLShaderParser();
		ShaderReflectionData ReflectShader(const std::string &source);
	};
}	 // namespace Nexus::OpenGL