#if defined(NX_PLATFORM_OPENGL)

#include "ShaderModuleOpenGL.hpp"

#include "Nexus/Logging/Log.hpp"

namespace Nexus::Graphics
{
    ShaderModuleOpenGL::ShaderModuleOpenGL(const ShaderModuleSpecification &shaderModuleSpec, const ResourceSetSpecification &resourceSpec)
        : ShaderModule(shaderModuleSpec, resourceSpec)
    {
        m_ShaderStage = GL::GetShaderStage(m_ModuleSpecification.Stage);
        m_Handle = glCreateShader(m_ShaderStage);
        const char *source = m_ModuleSpecification.Source.c_str();
        glShaderSource(m_Handle, 1, &source, nullptr);
        glCompileShader(m_Handle);

        int success;
        char infoLog[512];
        glGetShaderiv(m_Handle, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(m_Handle, 512, NULL, infoLog);
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
}

#endif