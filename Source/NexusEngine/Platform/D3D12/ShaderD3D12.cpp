#include "ShaderD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

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

    IDxcBlob *ShaderD3D12::GetVertexShaderBlob()
    {
        return m_VertexBlobPtr;
    }

    IDxcBlob *ShaderD3D12::GetFragmentShaderBlob()
    {
        return m_FragmentBlobPtr;
    }

    void ShaderD3D12::CreateVertexShader()
    {
        Microsoft::WRL::ComPtr<IDxcCompiler3> compiler;
        Microsoft::WRL::ComPtr<IDxcUtils> utils;
        Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler;

        DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf()));
        DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
        utils->CreateDefaultIncludeHandler(includeHandler.GetAddressOf());

        Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob;
        utils->CreateBlob(m_VertexShaderSource.c_str(), m_VertexShaderSource.length(), CP_UTF8, sourceBlob.GetAddressOf());

        std::vector<LPCWSTR> compilationArguments =
            {
                L"-E",
                L"vs_main",
                L"-T",
                L"vs_6_6",
                DXC_ARG_WARNINGS_ARE_ERRORS};

#if defined(DEBUG) || defined(_DEBUG)
        compilationArguments.push_back(DXC_ARG_DEBUG);
#endif

        DxcBuffer sourceBuffer{};
        sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
        sourceBuffer.Size = sourceBlob->GetBufferSize();
        sourceBuffer.Encoding = 0;

        Microsoft::WRL::ComPtr<IDxcResult> compiledShaderBuffer{};
        const HRESULT hr = compiler->Compile(
            &sourceBuffer,
            compilationArguments.data(),
            compilationArguments.size(),
            includeHandler.Get(),
            IID_PPV_ARGS(compiledShaderBuffer.GetAddressOf()));

        compiledShaderBuffer->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&m_VertexBlobPtr), nullptr);
    }

    void ShaderD3D12::CreateFragmentShader()
    {
        Microsoft::WRL::ComPtr<IDxcCompiler3> compiler;
        Microsoft::WRL::ComPtr<IDxcUtils> utils;
        Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler;

        DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf()));
        DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
        utils->CreateDefaultIncludeHandler(includeHandler.GetAddressOf());

        Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob;
        utils->CreateBlob(m_FragmentShaderSource.c_str(), m_FragmentShaderSource.length(), CP_UTF8, sourceBlob.GetAddressOf());

        std::vector<LPCWSTR> compilationArguments =
            {
                L"-E",
                L"ps_main",
                L"-T",
                L"ps_6_6",
                DXC_ARG_WARNINGS_ARE_ERRORS};

#if defined(DEBUG) || defined(_DEBUG)
        compilationArguments.push_back(DXC_ARG_DEBUG);
#endif

        DxcBuffer sourceBuffer{};
        sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
        sourceBuffer.Size = sourceBlob->GetBufferSize();
        sourceBuffer.Encoding = 0;

        Microsoft::WRL::ComPtr<IDxcResult> compiledShaderBuffer{};
        const HRESULT hr = compiler->Compile(
            &sourceBuffer,
            compilationArguments.data(),
            compilationArguments.size(),
            includeHandler.Get(),
            IID_PPV_ARGS(compiledShaderBuffer.GetAddressOf()));

        compiledShaderBuffer->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&m_FragmentBlobPtr), nullptr);
    }
}

#endif