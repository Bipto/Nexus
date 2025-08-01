#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::OpenGL
{
	struct ReflectedBufferMember
	{
		std::string Type		= {};
		std::string Name		= {};
		std::string ArraySuffix = {};
	};

	struct ReflectedCustomStructs
	{
		std::string						   Name	   = {};
		std::vector<ReflectedBufferMember> Members = {};
	};

	struct ReflectedShaderUniforms
	{
		std::string LayoutQualififers = {};
		std::string StorageQualifier = {};
		std::string Type			 = {};
		std::string Name			 = {};
		std::string ArraySuffix		 = {};
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
		std::vector<ReflectedCustomStructs>	 CustomStructs	= {};
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