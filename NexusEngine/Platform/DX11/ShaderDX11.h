#pragma once

#include "DX11.h"

#include "Core/Graphics/Shader.h"

#include <iostream>

namespace Nexus
{
    class ShaderDX11 : public Shader
    {
        public:
            ShaderDX11(ID3D11Device* device, ID3D11DeviceContext* context, std::string vertexShaderSource, std::string fragmentShaderSource, const BufferLayout& layout);
            ShaderDX11(ID3D11Device* device, ID3D11DeviceContext* context, const std::string& filepath, const BufferLayout& layout);
            virtual void Bind() override;
            virtual void SetShaderUniform1i(const std::string& name, int value) override{}
            virtual void SetShaderUniform1f(const std::string& name, float value) override{}
            virtual void SetShaderUniform2f(const std::string& name, const glm::vec2& value) override{}
            virtual void SetShaderUniform3f(const std::string& name, const glm::vec3& value) override{}
            virtual void SetShaderUniform4f(const std::string& name, const glm::vec4& value) override{}
            virtual void SetShaderUniformMat3(const std::string& name, const glm::mat3& value) override{}
            virtual void SetShaderUniformMat4(const std::string& name, const glm::mat4& value) override{}
            virtual const BufferLayout& GetLayout() const override { return m_BufferLayout; }

        private:
            void CreateLayout(const BufferLayout& layout);

        private:
            ID3D11Device* m_Device              = NULL;
            ID3D11DeviceContext* m_ContextPtr   = NULL;
            ID3D11InputLayout* m_InputLayout    = NULL;
            ID3D11VertexShader* m_VertexShader  = NULL;
            ID3D11PixelShader* m_PixelShader    = NULL;
            ID3DBlob* m_VertexBlobPtr           = NULL;
            ID3DBlob* m_PixelBlobPtr            = NULL;
            BufferLayout m_BufferLayout;
    };
}