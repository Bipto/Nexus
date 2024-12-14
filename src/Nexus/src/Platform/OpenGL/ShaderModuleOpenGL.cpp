#if defined(NX_PLATFORM_OPENGL)

	#include "ShaderModuleOpenGL.hpp"

	#include "Nexus-Core/Logging/Log.hpp"

namespace Nexus::Graphics
{
	ShaderModuleOpenGL::ShaderModuleOpenGL(const ShaderModuleSpecification &shaderModuleSpec, const ResourceSetSpecification &resourceSpec)
		: ShaderModule(shaderModuleSpec, resourceSpec),
		  m_ShaderStage(GL::GetShaderStage(m_ModuleSpecification.ShadingStage))
	{
		m_Handle		   = glCreateShader(m_ShaderStage);
		const char *source = m_ModuleSpecification.Source.c_str();
		glCall(glShaderSource(m_Handle, 1, &source, nullptr));
		glCall(glCompileShader(m_Handle));

		int success;
		glCall(glGetShaderiv(m_Handle, GL_COMPILE_STATUS, &success));

		if (!success)
		{
			char infoLog[512];
			glCall(glGetShaderInfoLog(m_Handle, 512, NULL, infoLog));
			std::string errorMessage = "Error: Vertex Shader - " + std::string(infoLog);
			NX_ERROR(errorMessage);
		}
	}

	GLenum ShaderModuleOpenGL::GetGLShaderStage()
	{
		return m_ShaderStage;
	}

	uint32_t ShaderModuleOpenGL::GetHandle()
	{
		return m_Handle;
	}
}	 // namespace Nexus::Graphics

#endif