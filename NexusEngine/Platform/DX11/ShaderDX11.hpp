#pragma once

#if defined(WIN32)

#include "DX11.hpp"

#include "Core/Graphics/Shader.hpp"

#include <iostream>

namespace Nexus::Graphics
{
    class ShaderDX11 : public Shader
    {
    public:
        ShaderDX11(ID3D11Device *device, ID3D11DeviceContext *context, std::string vertexShaderSource, std::string fragmentShaderSource, const VertexBufferLayout &layout);
        virtual void SetTexture(Ref<Texture> texture, const TextureBinding &binding) override;

        virtual const std::string &GetVertexShaderSource() override;
        virtual const std::string &GetFragmentShaderSource() override;

        ID3D11VertexShader *GetVertexShader() { return m_VertexShader; }
        ID3D11PixelShader *GetPixelShader() { return m_PixelShader; }
        ID3D11InputLayout *GetInputLayout() { return m_InputLayout; }

        virtual const VertexBufferLayout &GetLayout() const override { return m_BufferLayout; }
        virtual void BindUniformBuffer(Ref<UniformBuffer> buffer, const UniformResourceBinding &binding) override;

    private:
        void CreateLayout(const VertexBufferLayout &layout);

    private:
        ID3D11Device *m_Device = NULL;
        ID3D11DeviceContext *m_ContextPtr = NULL;
        ID3D11InputLayout *m_InputLayout = NULL;
        ID3D11VertexShader *m_VertexShader = NULL;
        ID3D11PixelShader *m_PixelShader = NULL;
        ID3DBlob *m_VertexBlobPtr = NULL;
        ID3DBlob *m_PixelBlobPtr = NULL;
        VertexBufferLayout m_BufferLayout;

        std::string m_VertexShaderSource;
        std::string m_FragmentShaderSource;
    };
}

#endif