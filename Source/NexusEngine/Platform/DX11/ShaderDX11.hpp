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
        ShaderDX11(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::string vertexShaderSource, std::string fragmentShaderSource, const VertexBufferLayout &layout);

        virtual const std::string &GetVertexShaderSource() override;
        virtual const std::string &GetFragmentShaderSource() override;

        Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader() { return m_VertexShader; }
        Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader() { return m_PixelShader; }
        Microsoft::WRL::ComPtr<ID3D11InputLayout> GetInputLayout() { return m_InputLayout; }

        virtual const VertexBufferLayout &GetLayout() const override { return m_BufferLayout; }

    private:
        void CreateLayout(const VertexBufferLayout &layout);

    private:
        Microsoft::WRL::ComPtr<ID3D11Device> m_Device = NULL;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_ContextPtr = NULL;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout = NULL;
        Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader = NULL;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader = NULL;
        Microsoft::WRL::ComPtr<ID3DBlob> m_VertexBlobPtr = NULL;
        Microsoft::WRL::ComPtr<ID3DBlob> m_PixelBlobPtr = NULL;
        VertexBufferLayout m_BufferLayout;

        std::string m_VertexShaderSource;
        std::string m_FragmentShaderSource;
    };
}

#endif