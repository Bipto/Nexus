#pragma once

#include "D3D12Include.hpp"
#include "Nexus/Graphics/Shader.hpp"

namespace Nexus::Graphics
{
    class ShaderD3D12 : public Shader
    {
    public:
        ShaderD3D12(ID3D12Device10 *device, std::string vertexShaderSource, std::string fragmentShaderSource, const VertexBufferLayout &layout);
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
        ID3D12Device10 *m_Device;

        VertexBufferLayout m_BufferLayout;
        std::string m_VertexShaderSource;
        std::string m_FragmentShaderSource;

        ID3DBlob *m_VertexBlobPtr = nullptr;
        ID3DBlob *m_FragmentBlobPtr = nullptr;
    };
}