#pragma once

#include "Nexus-Core/Graphics/Multisample.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::GL
{
	enum class OpenGLVersion
	{
		OpenGL,
		OpenGLES
	};

	struct ContextSpecification
	{
		uint8_t				  VersionMajor	 = 4;
		uint8_t				  VersionMinor	 = 6;
		bool				  DoubleBuffered = true;
		bool				  Debug			 = false;
		uint8_t				  DepthBits		 = 24;
		uint8_t				  StencilBits	 = 8;
		uint8_t				  RedBits		 = 8;
		uint8_t				  GreenBits		 = 8;
		uint8_t				  BlueBits		 = 8;
		uint8_t				  AlphaBits		 = 8;
		Graphics::SampleCount Samples		 = Graphics::SampleCount::SampleCount1;
		bool				  Vsync			 = true;
		bool				  UseCoreProfile = true;
		OpenGLVersion		  GLVersion		 = OpenGLVersion::OpenGL;
	};
}	 // namespace Nexus::GL