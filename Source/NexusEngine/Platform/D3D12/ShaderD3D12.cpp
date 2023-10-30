#include "ShaderD3D12.hpp"

#include <d3dcompiler.h>

namespace Nexus::Graphics
{
    Nexus::Graphics::ShaderD3D12::ShaderD3D12(std::string vertexShaderSource, std::string fragmentShaderSource, const VertexBufferLayout &layout)
        : m_VertexShaderSource(vertexShaderSource), m_FragmentShaderSource(fragmentShaderSource), m_BufferLayout(layout)
    {
        CreateVertexShader();
        CreateFragmentShader();
    }

    ShaderD3D12::~ShaderD3D12()
    {
    }

    const std::string &ShaderD3D12::GetVertexShaderSource()
    {
        return m_VertexShaderSource;
    }

    const std::string &ShaderD3D12::GetFragmentShaderSource()
    {
        return m_FragmentShaderSource;
    }

    ID3DBlob *ShaderD3D12::GetVertexShaderBlob()
    {
        return m_VertexBlobPtr;
    }

    ID3DBlob *ShaderD3D12::GetFragmentShaderBlob()
    {
        return m_FragmentBlobPtr;
    }

    void ShaderD3D12::CreateVertexShader()
    {
        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
        flags |= D3DCOMPILE_DEBUG;
#endif

        ID3DBlob *error_blob = nullptr;

        HRESULT hr = D3DCompile(
            m_VertexShaderSource.c_str(),
            m_VertexShaderSource.length(),
            "shader.vert",
            nullptr,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "vs_main",
            "vs_5_0",
            flags,
            0,
            &m_VertexBlobPtr,
            &error_blob);
    }

    void ShaderD3D12::CreateFragmentShader()
    {
        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
        flags |= D3DCOMPILE_DEBUG;
#endif

        ID3DBlob *error_blob = nullptr;

        HRESULT hr = D3DCompile(
            m_FragmentShaderSource.c_str(),
            m_FragmentShaderSource.length(),
            "shader.frag",
            nullptr,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "ps_main",
            "ps_5_0",
            flags,
            0,
            &m_FragmentBlobPtr,
            &error_blob);
    }
}