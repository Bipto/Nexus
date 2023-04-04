#pragma once

#include "DX11.h"

#include "Core/Graphics/Shader.h"

#include <iostream>

namespace Nexus
{
    static DXGI_FORMAT GetDXBaseType(const BufferElement element)
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

    class ShaderDX11 : public Shader
    {
        public:
            ShaderDX11(ID3D11Device* device, ID3D11DeviceContext* context, const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const BufferLayout& layout)
            {
                m_Device = device;
                m_ContextPtr = context;
                CreateLayout(layout);
            }

            ShaderDX11(ID3D11Device* device, ID3D11DeviceContext* context, const std::string& filepath, const BufferLayout& layout)
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
                        std::cout << (char*)error_blob->GetBufferPointer() << std::endl;
                        error_blob->Release();
                    }
                    if (m_VertexBlobPtr) { m_VertexBlobPtr->Release(); }
                }
                else
                {
                    std::cout << "Vertex shader loaded successfully\n";

                    hr = device->CreateVertexShader(m_VertexBlobPtr->GetBufferPointer(), m_VertexBlobPtr->GetBufferSize(), NULL, &m_VertexShader);
                    if (FAILED(hr))
                    {
                        std::cout << "Failed to create vertex shader\n";
                    }
                    else
                    {
                        std::cout << "Vertex shader created successfully\n";
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
                        std::cout << (char*)error_blob->GetBufferPointer() << std::endl;
                        error_blob->Release();
                    }
                    if (m_PixelBlobPtr) { m_PixelBlobPtr->Release(); }
                }
                else
                {
                    std::cout << "Pixel shader loaded successfully\n";

                    hr = device->CreatePixelShader(m_PixelBlobPtr->GetBufferPointer(), m_PixelBlobPtr->GetBufferSize(), NULL, &m_PixelShader);
                    if (FAILED(hr))
                    {
                        std::cout << "Failed to create pixel shader\n";
                    }
                    else
                    {
                        std::cout << "Pixel shader created successfully\n";
                    }
                }

                CreateLayout(layout);
            }

            virtual void Bind() override
            {
                m_ContextPtr->VSSetShader(m_VertexShader, 0, 0);
                m_ContextPtr->PSSetShader(m_PixelShader, 0, 0);
                m_ContextPtr->IASetInputLayout(m_InputLayout);
            }

            virtual void SetShaderUniform1i(const std::string& name, int value) override
            {

            }

            virtual void SetShaderUniform1f(const std::string& name, float value) override
            {

            }

            virtual void SetShaderUniform2f(const std::string& name, const glm::vec2& value) override
            {

            }

            virtual void SetShaderUniform3f(const std::string& name, const glm::vec3& value) override
            {

            }

            virtual void SetShaderUniform4f(const std::string& name, const glm::vec4& value) override
            {

            }

            virtual void SetShaderUniformMat3(const std::string& name, const glm::mat3& value) override
            {

            }

            virtual void SetShaderUniformMat4(const std::string& name, const glm::mat4& value) override
            {

            }

            virtual const BufferLayout& GetLayout() const override
            {
                return m_BufferLayout;
            }

        private:
            void CreateLayout(const BufferLayout& layout)
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
                    std::cout << "Failed to create input layout: ";
                    std::cout << error.ErrorMessage() << std::endl;
                }
                else
                {
                    std::cout << "Input layout created successfully\n";
                }
            }

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