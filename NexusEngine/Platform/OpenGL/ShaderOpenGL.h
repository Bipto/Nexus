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

    GLenum GetGLBaseType(const BufferElement element);

    class ShaderOpenGL : public Shader
    {
        public:
            ShaderOpenGL(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const BufferLayout& layout);            virtual void Bind() override;
            virtual void SetShaderUniform1i(const std::string& name, int value) override;
            virtual void SetShaderUniform1f(const std::string& name, float value) override;
            virtual void SetShaderUniform2f(const std::string& name, const glm::vec2& value) override;
            virtual void SetShaderUniform3f(const std::string& name, const glm::vec3& value) override;
            virtual void SetShaderUniform4f(const std::string& name, const glm::vec4& value) override;
            virtual void SetShaderUniformMat3(const std::string& name, const glm::mat3& value) override;
            virtual void SetShaderUniformMat4(const std::string& name, const glm::mat4& value) override;
            virtual void SetTexture(Ref<Texture> texture, int slot) override;
            
            virtual const std::string& GetVertexShaderSource() override;
            virtual const std::string& GetFragmentShaderSource() override;
            
            virtual const BufferLayout& GetLayout() const override;
        private:
            void Compile(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
            void SetLayout();

        private:
            unsigned int m_ProgramHandle;
            BufferLayout m_Layout;

            std::string m_VertexShaderSource;
            std::string m_FragmentShaderSource;
    };
}