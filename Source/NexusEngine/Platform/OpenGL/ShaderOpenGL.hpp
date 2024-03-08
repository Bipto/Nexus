#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "GL.hpp"
#include "Nexus/Graphics/Shader.hpp"
#include "Nexus/nxpch.hpp"

#include <iostream>

#include <filesystem>

namespace Nexus::Graphics
{
    struct ShaderSources
    {
        std::string VertexSource;
        std::string FragmentSource;
    };

    GLenum GetGLBaseType(const VertexBufferElement element);

    class ShaderOpenGL : public Shader
    {
    public:
        ShaderOpenGL(const std::string &vertexShaderSource, const std::string &fragmentShaderSource);
        virtual ~ShaderOpenGL();
        void Bind();

        virtual const std::string &GetVertexShaderSource() override;
        virtual const std::string &GetFragmentShaderSource() override;

        unsigned int GetHandle() { return m_ProgramHandle; }

    private:
        void Compile(const std::string &vertexShaderSource, const std::string &fragmentShaderSource);

    private:
        unsigned int m_ProgramHandle;
        std::string m_VertexShaderSource;
        std::string m_FragmentShaderSource;
    };
}

#endif