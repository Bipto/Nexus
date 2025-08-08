#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "GL.hpp"
	#include "Nexus-Core/Graphics/ShaderModule.hpp"

namespace Nexus::Graphics
{
	class GraphicsDeviceOpenGL;

	class ShaderModuleOpenGL : public ShaderModule
	{
	  public:
		ShaderModuleOpenGL(const ShaderModuleSpecification &shaderModuleSpec, GraphicsDeviceOpenGL *device);
		virtual ~ShaderModuleOpenGL();
		GLenum	 GetGLShaderStage();
		uint32_t GetHandle();
		ShaderReflectionData Reflect() const final;

	  private:
		GraphicsDeviceOpenGL *m_Device		= nullptr;
		uint32_t m_Handle	   = 0;
		GLenum	 m_ShaderStage = 0;
	};
}	 // namespace Nexus::Graphics

#endif