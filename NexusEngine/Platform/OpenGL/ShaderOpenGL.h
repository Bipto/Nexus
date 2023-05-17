#pragma once

#include "GL.h"
#include "Core/Graphics/Shader.h"
#include "Core/nxpch.h"

#include <iostream>

#include <filesystem>

namespace Nexus
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
            ShaderOpenGL(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const VertexBufferLayout& layout);
            void Bind();
            virtual void SetTexture(Ref<Texture> texture, const TextureBinding& binding) override;
            
            virtual const std::string& GetVertexShaderSource() override;
            virtual const std::string& GetFragmentShaderSource() override;

            virtual const VertexBufferLayout& GetLayout() const override;

            unsigned int GetHandle() { return m_ProgramHandle; }
        private:
            void Compile(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
            void SetLayout();

        private:
            unsigned int m_ProgramHandle;
            VertexBufferLayout m_Layout;

            std::string m_VertexShaderSource;
            std::string m_FragmentShaderSource;
    };
}