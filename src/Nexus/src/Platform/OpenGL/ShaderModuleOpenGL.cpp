#if defined(NX_PLATFORM_OPENGL)

	#include "ShaderModuleOpenGL.hpp"

	#include "Nexus-Core/Logging/Log.hpp"

	#include "GraphicsDeviceOpenGL.hpp"

namespace Nexus::Graphics
{
	ShaderModuleOpenGL::ShaderModuleOpenGL(const ShaderModuleSpecification &shaderModuleSpec,
										   const ResourceSetSpecification  &resourceSpec,
										   GraphicsDeviceOpenGL			   *device)
		: ShaderModule(shaderModuleSpec, resourceSpec),
		  m_ShaderStage(GL::GetShaderStage(m_ModuleSpecification.ShadingStage)),
		  m_Device(device)
	{
		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				m_Handle		   = context.CreateShader(m_ShaderStage);
				const char *source = m_ModuleSpecification.Source.c_str();
				glCall(context.ShaderSource(m_Handle, 1, &source, nullptr));
				glCall(context.CompileShader(m_Handle));

				int success;
				glCall(context.GetShaderiv(m_Handle, GL_COMPILE_STATUS, &success));

				if (!success)
				{
					char infoLog[512];
					glCall(context.GetShaderInfoLog(m_Handle, 512, NULL, infoLog));
					std::string errorMessage = "Error: Vertex Shader - " + std::string(infoLog);
					NX_ERROR(errorMessage);
				}
			});
	}

	ShaderModuleOpenGL::~ShaderModuleOpenGL()
	{
		GL::IOffscreenContext *offscreenContext = m_Device->GetOffscreenContext();
		GL::ExecuteGLCommands([&](const GladGLContext &context) { context.DeleteShader(m_Handle); });
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