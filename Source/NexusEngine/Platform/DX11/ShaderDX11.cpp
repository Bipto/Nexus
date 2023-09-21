#if defined(WIN32)

#include "ShaderDX11.hpp"
#include "Nexus/Logging/Log.hpp"
#include "TextureDX11.hpp"
#include "BufferDX11.hpp"

namespace Nexus::Graphics
{
    DXGI_FORMAT GetDXBaseType(const VertexBufferElement element)
    {
        switch (element.Type)
        {
        case ShaderDataType::Float:
            return DXGI_FORMAT_R32_FLOAT;
        case ShaderDataType::Float2:
            return DXGI_FORMAT_R32G32_FLOAT;
        case ShaderDataType::Float3:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case ShaderDataType::Float4:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case ShaderDataType::Int:
            return DXGI_FORMAT_R32_SINT;
        case ShaderDataType::Int2:
            return DXGI_FORMAT_R32G32_SINT;
        case ShaderDataType::Int3:
            return DXGI_FORMAT_R32G32B32_SINT;
        case ShaderDataType::Int4:
            return DXGI_FORMAT_R32G32B32A32_SINT;
        }
    }

    ShaderDX11::ShaderDX11(ID3D11Device *device, ID3D11DeviceContext *context, std::string vertexShaderSource, std::string fragmentShaderSource, const VertexBufferLayout &layout)
    {
        m_Device = device;
        m_ContextPtr = context;

        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
        flags |= D3DCOMPILE_DEBUG;
#endif

        ID3DBlob *error_blob = NULL;

        HRESULT hr = D3DCompile(
            vertexShaderSource.c_str(),
            vertexShaderSource.length(),
            "shader.vert",
            nullptr,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "vs_main",
            "vs_5_0",
            flags,
            0,
            &m_VertexBlobPtr,
            &error_blob);

        if (FAILED(hr))
        {
            if (error_blob)
            {
                std::string errorMessage = std::string((char *)error_blob->GetBufferPointer());
                NX_ERROR(errorMessage);
                error_blob->Release();
            }
        }
        else
        {
            hr = device->CreateVertexShader(m_VertexBlobPtr->GetBufferPointer(), m_VertexBlobPtr->GetBufferSize(), NULL, &m_VertexShader);
            if (FAILED(hr))
            {
                NX_ERROR("Failed to create vertex shader");
            }
            else
            {
                CreateLayout(layout);
            }
        }

        hr = D3DCompile(
            fragmentShaderSource.c_str(),
            fragmentShaderSource.length(),
            "shader.frag",
            nullptr,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "ps_main",
            "ps_5_0",
            flags,
            0,
            &m_PixelBlobPtr,
            &error_blob);

        if (FAILED(hr))
        {
            if (error_blob)
            {
                std::string errorMessage = std::string((char *)error_blob->GetBufferPointer());
                NX_ERROR(errorMessage);
                error_blob->Release();
            }
        }
        else
        {
            hr = device->CreatePixelShader(m_PixelBlobPtr->GetBufferPointer(), m_PixelBlobPtr->GetBufferSize(), NULL, &m_PixelShader);
            if (FAILED(hr))
            {
                NX_ERROR("Failed to create pixel shader");
            }
        }

        m_VertexShaderSource = vertexShaderSource;
        m_FragmentShaderSource = fragmentShaderSource;
    }

    ShaderDX11::~ShaderDX11()
    {
        m_VertexBlobPtr->Release();
        m_PixelBlobPtr->Release();
        m_VertexShader->Release();
        m_PixelShader->Release();
        m_InputLayout->Release();
    }

    const std::string &ShaderDX11::GetVertexShaderSource()
    {
        return m_VertexShaderSource;
    }

    const std::string &ShaderDX11::GetFragmentShaderSource()
    {
        return m_FragmentShaderSource;
    }

    void ShaderDX11::CreateLayout(const VertexBufferLayout &layout)
    {
        m_BufferLayout = layout;

        std::vector<D3D11_INPUT_ELEMENT_DESC> elementDescriptions;
        unsigned int index = 0;
        for (auto &element : m_BufferLayout)
        {
            D3D11_INPUT_ELEMENT_DESC desc =
                {
                    element.Name.c_str(),
                    index,
                    GetDXBaseType(element),
                    0,
                    D3D11_APPEND_ALIGNED_ELEMENT,
                    D3D11_INPUT_PER_VERTEX_DATA,
                    0};
            elementDescriptions.push_back(desc);

            index++;
        }

        HRESULT hr = m_Device->CreateInputLayout(
            elementDescriptions.data(),
            elementDescriptions.size(),
            m_VertexBlobPtr->GetBufferPointer(),
            m_VertexBlobPtr->GetBufferSize(),
            &m_InputLayout);

        if (FAILED(hr))
        {
            _com_error error(hr);
            std::string errorMessage = std::string("Failed to create input layout: ") + std::string(error.ErrorMessage());
            NX_ERROR(errorMessage);
        }
    }
}

#endif