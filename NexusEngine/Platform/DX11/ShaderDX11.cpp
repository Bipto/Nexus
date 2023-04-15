#include "ShaderDX11.h"

#include "Core/Logging/Log.h"

namespace Nexus
{
    DXGI_FORMAT GetDXBaseType(const BufferElement element)
    {
        switch (element.Type)
        {
            case ShaderDataType::Float:     return DXGI_FORMAT_R32_FLOAT;
            case ShaderDataType::Float2:    return DXGI_FORMAT_R32G32_FLOAT;
            case ShaderDataType::Float3:    return DXGI_FORMAT_R32G32B32_FLOAT;
            case ShaderDataType::Float4:    return DXGI_FORMAT_R32G32B32A32_FLOAT;
            case ShaderDataType::Int:       return DXGI_FORMAT_R32_SINT;
            case ShaderDataType::Int2:      return DXGI_FORMAT_R32G32_SINT;
            case ShaderDataType::Int3:      return DXGI_FORMAT_R32G32B32_SINT;
            case ShaderDataType::Int4:      return DXGI_FORMAT_R32G32B32A32_SINT;
        }
    }

    ShaderDX11::ShaderDX11(ID3D11Device* device, ID3D11DeviceContext* context, std::string vertexShaderSource, std::string fragmentShaderSource, const BufferLayout& layout)
    {
        m_Device = device;
        m_ContextPtr = context;

        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
        #if defined(DEBUG) || defined(_DEBUG)
            flags |= D3DCOMPILE_DEBUG;
        #endif

        ID3DBlob* error_blob = NULL;

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
            &error_blob
        );

        if (FAILED(hr))
        {
            if (error_blob)
            {
                std::string errorMessage = std::string((char*)error_blob->GetBufferPointer());
                NX_ERROR(errorMessage);
                error_blob->Release();
            }
            if (m_VertexBlobPtr) { m_VertexBlobPtr->Release(); }
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
                NX_LOG("Vertex shader created successfully");
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
            &error_blob
        );

        if (FAILED(hr))
        {
            if (error_blob)
            {
                std::string errorMessage = std::string((char*)error_blob->GetBufferPointer());
                NX_ERROR(errorMessage);
                error_blob->Release();
            }
            if (m_PixelBlobPtr) { m_PixelBlobPtr->Release(); }
        }
        else
        {
            hr = device->CreatePixelShader(m_PixelBlobPtr->GetBufferPointer(), m_PixelBlobPtr->GetBufferSize(), NULL, &m_PixelShader);
            if (FAILED(hr))
            {
                NX_ERROR("Failed to create pixel shader");
            }
            else
            {
                NX_LOG("Pixel shader created successfully");
            }
        }
    }

    ShaderDX11::ShaderDX11(ID3D11Device* device, ID3D11DeviceContext* context, const std::string& filepath, const BufferLayout& layout)
    {
        m_Device = device;
        m_ContextPtr = context;

        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
        #if defined(DEBUG) || defined(_DEBUG)
            flags |= D3DCOMPILE_DEBUG;
        #endif

        ID3DBlob *error_blob = NULL;

        //compile vertex shader
        HRESULT hr = D3DCompileFromFile(
            L"shaders.hlsl",
            nullptr,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "vs_main",
            "vs_5_0",
            flags,
            0,
            &m_VertexBlobPtr,
            &error_blob  
        );

        if (FAILED(hr))
        {
            if (error_blob)
            {
                std::string errorMessage = std::string((char*)error_blob->GetBufferPointer());
                NX_ERROR(errorMessage);
                error_blob->Release();
            }
            if (m_VertexBlobPtr) { m_VertexBlobPtr->Release(); }
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
                NX_LOG("Vertex shader created successfully");
            }
        }

        //compile vertex shader
        hr = D3DCompileFromFile(
            L"shaders.hlsl",
            nullptr,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "ps_main",
            "ps_5_0",
            flags,
            0,
            &m_PixelBlobPtr,
            &error_blob
        );
        if (FAILED(hr))
        {
            if (error_blob)
            {
                std::string errorMessage = std::string((char*)error_blob->GetBufferPointer());
                NX_ERROR(errorMessage);
                error_blob->Release();
            }
            if (m_PixelBlobPtr) { m_PixelBlobPtr->Release(); }
        }
        else
        {
            hr = device->CreatePixelShader(m_PixelBlobPtr->GetBufferPointer(), m_PixelBlobPtr->GetBufferSize(), NULL, &m_PixelShader);
            if (FAILED(hr))
            {
                NX_ERROR("Failed to create pixel shader");
            }
            else
            {
                NX_LOG("Pixel shader created successfully");
            }
        }

        CreateLayout(layout);
    }

    void ShaderDX11::Bind()
    {
        m_ContextPtr->VSSetShader(m_VertexShader, 0, 0);
        m_ContextPtr->PSSetShader(m_PixelShader, 0, 0);
        m_ContextPtr->IASetInputLayout(m_InputLayout);
    }

    void ShaderDX11::CreateLayout(const BufferLayout& layout)
    {
        m_BufferLayout = layout;
            
        std::vector<D3D11_INPUT_ELEMENT_DESC> elementDescriptions;
        unsigned int index = 0;
        for (auto& element : m_BufferLayout)
        {
            D3D11_INPUT_ELEMENT_DESC desc = 
            {
                element.Name.c_str(),
                0,
                GetDXBaseType(element),
                0,
                D3D11_APPEND_ALIGNED_ELEMENT,
                D3D11_INPUT_PER_VERTEX_DATA,
                0
            };
            elementDescriptions.push_back(desc);

            index++;
        }

        HRESULT hr = m_Device->CreateInputLayout(
            elementDescriptions.data(),
            elementDescriptions.size(),
            m_VertexBlobPtr->GetBufferPointer(),
            m_VertexBlobPtr->GetBufferSize(),
            &m_InputLayout
        );

        if (FAILED(hr))
        {
            _com_error error(hr);
            std::string errorMessage = std::string("Failed to create input layout: ") + std::string(error.ErrorMessage());
            NX_ERROR(errorMessage);
        }
        else
        {
            NX_LOG("Input layout created successfully");
        }
    }
}