#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "GL.hpp"
#include "Nexus/Graphics/ShaderModule.hpp"

namespace Nexus::Graphics
{
    class ShaderModuleOpenGL : public ShaderModule
    {
    public:
        ShaderModuleOpenGL(const ShaderModuleSpecification &shaderModuleSpec, const ResourceSetSpecification &resourceSpec);
        GLenum GetGLShaderStage();
        uint32_t GetHandle();

    private:
        uint32_t m_Handle = 0;
        GLenum m_ShaderStage = 0;
    };
}

#endif