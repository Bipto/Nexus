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

	struct ReflectedBufferMember
	{
		std::string Type		= {};
		std::string Name		= {};
		std::string ArraySuffix = {};
	};

	struct ReflectedShaderBuffer
	{
		std::string							LayoutQualifiers = {};
		std::string							StorageQualifier = {};
		std::string							BlockName		 = {};
		std::string							InstanceName	 = {};
		std::vector<ReflectedBufferMember>	Members			 = {};
	};

	struct ShaderReflectionData
	{
		std::vector<ReflectedShaderUniforms> Uniforms		= {};
		std::vector<ReflectedShaderBuffer>	 Buffers		= {};
	};

	class OpenGLShaderParser
	{
	  public:
		OpenGLShaderParser();
		ShaderReflectionData ReflectShader(const std::string &source);
	};
}	 // namespace Nexus::OpenGL