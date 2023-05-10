#include "BufferDX11.h"

#include "Core/Logging/Log.h"

namespace Nexus
{
    VertexBufferDX11::VertexBufferDX11(ID3D11Device* device, const std::vector<float>& vertices)
        : VertexBuffer(vertices)
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));

        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = vertices.size() * sizeof(float);
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bd.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA vbData;
        ZeroMemory(&vbData, sizeof(vbData));
        vbData.pSysMem = vertices.data();

        HRESULT hr = device->CreateBuffer(&bd, &vbData, &m_VertexBuffer);

        if (FAILED(hr))
        {
            _com_error error(hr);
            std::string errorMessage = std::string("Failed to create vertex buffer: ") + std::string(error.ErrorMessage());
            NX_ERROR(errorMessage);
        }

        m_VertexCount = vertices.size();
    }

    IndexBufferDX11::IndexBufferDX11(ID3D11Device* device, const std::vector<unsigned int>& indices)
        : IndexBuffer(indices)
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));

        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = indices.size() * sizeof(unsigned int);
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bd.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA ibData;
        ZeroMemory(&ibData, sizeof(ibData));
        ibData.pSysMem = indices.data();

        HRESULT hr = device->CreateBuffer(&bd, &ibData, &m_IndexBuffer);

        if (FAILED(hr))
        {
            _com_error error(hr);
            std::string errorMessage = std::string("Failed to create index buffer: ") + std::string(error.ErrorMessage());
            NX_ERROR(errorMessage);
        }

        m_IndexCount = indices.size();
    }

    UniformBufferDX11::UniformBufferDX11(ID3D11Device* device, ID3D11DeviceContext* context, uint32_t size, uint32_t binding)
        : UniformBuffer(size, binding)
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));

        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = size;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;

        HRESULT hr = device->CreateBuffer(&bd, NULL, &m_ConstantBuffer);

        if (FAILED(hr))
        {
            _com_error error(hr);
            std::string errorMessage = std::string("Failed to create constant buffer: ") + std::string(error.ErrorMessage());
            NX_ERROR(errorMessage);
        }

        m_DeviceContext = context;
        m_Binding = binding;
    }

    UniformBufferDX11::~UniformBufferDX11()
    {
        m_ConstantBuffer->Release();
        m_ConstantBuffer = nullptr;
    }

    void UniformBufferDX11::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        m_DeviceContext->UpdateSubresource(m_ConstantBuffer, 0, 0, data, 0, 0);

        m_DeviceContext->VSSetConstantBuffers(
            m_Binding,
            1,
            &m_ConstantBuffer
        );
    }
}