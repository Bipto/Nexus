#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::OpenGL
{
	struct ReflectedBufferMember
	{
		std::string				Type	  = {};
		std::string				Name	  = {};
		std::optional<uint32_t> ArraySize = {};
	};

	struct ReflectedCustomStructs
	{
		std::string						   Name	   = {};
		std::vector<ReflectedBufferMember> Members = {};
	};

	struct ReflectedShaderResource
	{
		std::string				LayoutQualififers = {};
		std::string				MemoryQualififers = {};
		std::string				StorageQualifier  = {};
		std::string				Type			  = {};
		std::string				Name			  = {};
		std::optional<uint32_t> ArraySize		  = {};
	};

	struct ReflectedShaderBuffer
	{
		std::string						   LayoutQualifiers	 = {};
		std::string						   MemoryQualififers = {};
		std::string						   StorageQualifier	 = {};
		std::string						   BlockName		 = {};
		std::string						   InstanceName		 = {};
		std::vector<ReflectedBufferMember> Members			 = {};
	};

	struct ReflectedShaderResources
	{
		std::vector<ReflectedCustomStructs>	 CustomStructs = {};
		std::vector<ReflectedShaderResource> Uniforms	   = {};
		std::vector<ReflectedShaderBuffer>	 Buffers	   = {};
	};

	class OpenGLShaderParser
	{
	  public:
		OpenGLShaderParser();
		ReflectedShaderResources ReflectShader(const std::string &source);
	};
}	 // namespace Nexus::OpenGL