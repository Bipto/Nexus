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

        ID3DBlob *GetVertexShaderBlob();
        ID3DBlob *GetFragmentShaderBlob();

    private:
        void CreateVertexShader();
        void CreateFragmentShader();

    private:
        VertexBufferLayout m_BufferLayout;
        std::string m_VertexShaderSource;
        std::string m_FragmentShaderSource;

        ID3DBlob *m_VertexBlobPtr = nullptr;
        ID3DBlob *m_FragmentBlobPtr = nullptr;
    };
}

#endif