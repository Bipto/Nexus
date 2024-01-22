#pragma once

#if defined(NX_PLATFORM_D3D12)

#include "D3D12Include.hpp"
#include "Nexus/Graphics/Shader.hpp"

namespace Nexus::Graphics
{
    class ShaderD3D12 : public Shader
    {
    public:
        ShaderD3D12(std::string vertexShaderSource, std::string fragmentShaderSource, const VertexBufferLayout &layout);
        virtual ~ShaderD3D12();

        virtual const std::string &GetVertexShaderSource() override;
        virtual const std::string &GetFragmentShaderSource() override;

        virtual const VertexBufferLayout &GetLayout() const override { return m_BufferLayout; }

        IDxcBlob *GetVertexShaderBlob();
        IDxcBlob *GetFragmentShaderBlob();

    private:
        void CreateVertexShader();
        void CreateFragmentShader();

    private:
        VertexBufferLayout m_BufferLayout;
        std::string m_VertexShaderSource;
        std::string m_FragmentShaderSource;

        IDxcBlob *m_VertexBlobPtr = nullptr;
        IDxcBlob *m_FragmentBlobPtr = nullptr;
    };
}

#endif