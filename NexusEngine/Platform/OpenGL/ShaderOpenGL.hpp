#pragma once

#include "GL.hpp"
#include "Core/Graphics/Shader.hpp"
#include "Core/nxpch.hpp"

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
        ShaderOpenGL(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const VertexBufferLayout &layout);
        void Bind();
        virtual void SetTexture(Ref<Texture> texture, const TextureBinding &binding) override;

        virtual const std::string &GetVertexShaderSource() override;
        virtual const std::string &GetFragmentShaderSource() override;

        virtual const VertexBufferLayout &GetLayout() const override;
        virtual void BindUniformBuffer(Ref<UniformBuffer> buffer, const UniformResourceBinding &binding) override;

        unsigned int GetHandle() { return m_ProgramHandle; }
        void SetLayout();

    private:
        void Compile(const std::string &vertexShaderSource, const std::string &fragmentShaderSource);

    private:
        unsigned int m_ProgramHandle;
        VertexBufferLayout m_Layout;

        std::string m_VertexShaderSource;
        std::string m_FragmentShaderSource;
    };
}