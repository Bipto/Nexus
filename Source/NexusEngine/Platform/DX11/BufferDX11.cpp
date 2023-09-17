#if defined(WIN32)

#include "BufferDX11.hpp"
#include "Core/Logging/Log.hpp"

namespace Nexus::Graphics
{
    D3D11_USAGE GetBufferUsage(BufferUsage usage)
    {
        switch (usage)
        {
        case BufferUsage::Dynamic:
            return D3D11_USAGE_DYNAMIC;
        case BufferUsage::Static:
            return D3D11_USAGE_DEFAULT;
        default:
            throw std::runtime_error("Invalid buffer usage entered");
        }
    }

    VertexBufferDX11::VertexBufferDX11(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, const BufferDescription &description, const void *data, const VertexBufferLayout &layout)
        : VertexBuffer(description, data, layout)
    {
        m_Context = context;
        auto bufferUsage = GetBufferUsage(m_Description.Usage);

        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));

        bd.Usage = bufferUsage;
        bd.ByteWidth = m_Description.Size;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        if (m_Description.Usage == BufferUsage::Static)
            bd.CPUAccessFlags = 0;
        else
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bd.MiscFlags = 0;

        HRESULT hr{};
        if (data)
        {
            D3D11_SUBRESOURCE_DATA bufferData;
            ZeroMemory(&bufferData, sizeof(bufferData));
            bufferData.pSysMem = data;
            HRESULT hr = device->CreateBuffer(&bd, &bufferData, &m_Buffer);
        }
        else
        {
            HRESULT hr = device->CreateBuffer(&bd, nullptr, &m_Buffer);
        }

        if (FAILED(hr))
        {
            _com_error error(hr);
            std::string errorMessage = std::string("Failed to create vertex buffer: ") + std::string(error.ErrorMessage());
            NX_ERROR(errorMessage);
        }
    }

    VertexBufferDX11::~VertexBufferDX11()
    {
    }

    void VertexBufferDX11::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
        m_Context->Map(m_Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

        unsigned char *buffer = reinterpret_cast<unsigned char *>(mappedResource.pData);
        buffer += offset;

        memcpy(buffer, data, size);

        m_Context->Unmap(m_Buffer.Get(), 0);
    }

    Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBufferDX11::GetHandle()
    {
        return m_Buffer;
    }

    IndexBufferDX11::IndexBufferDX11(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, const BufferDescription &description, const void *data, IndexBufferFormat format)
        : IndexBuffer(description, data, format)
    {
        m_Context = context;
        auto bufferUsage = GetBufferUsage(m_Description.Usage);

        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));

        bd.Usage = bufferUsage;
        bd.ByteWidth = m_Description.Size;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

        if (m_Description.Usage == BufferUsage::Static)
            bd.CPUAccessFlags = 0;
        else
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bd.MiscFlags = 0;

        HRESULT hr{};
        if (data)
        {
            D3D11_SUBRESOURCE_DATA bufferData;
            ZeroMemory(&bufferData, sizeof(bufferData));
            bufferData.pSysMem = data;
            HRESULT hr = device->CreateBuffer(&bd, &bufferData, &m_Buffer);
        }
        else
        {
            HRESULT hr = device->CreateBuffer(&bd, nullptr, &m_Buffer);
        }

        if (FAILED(hr))
        {
            _com_error error(hr);
            std::string errorMessage = std::string("Failed to create vertex buffer: ") + std::string(error.ErrorMessage());
            NX_ERROR(errorMessage);
        }
    }

    IndexBufferDX11::~IndexBufferDX11()
    {
    }

    void IndexBufferDX11::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
        m_Context->Map(m_Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

        unsigned char *buffer = reinterpret_cast<unsigned char *>(mappedResource.pData);
        buffer += offset;

        memcpy(buffer, data, size);

        m_Context->Unmap(m_Buffer.Get(), 0);
    }

    Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBufferDX11::GetHandle()
    {
        return m_Buffer;
    }

    UniformBufferDX11::UniformBufferDX11(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, const BufferDescription &description, const void *data)
        : UniformBuffer(description, data)
    {
        m_Context = context;
        auto bufferUsage = GetBufferUsage(m_Description.Usage);

        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));

        bd.Usage = bufferUsage;
        bd.ByteWidth = m_Description.Size;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

        if (m_Description.Usage == BufferUsage::Static)
            bd.CPUAccessFlags = 0;
        else
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bd.MiscFlags = 0;

        HRESULT hr{};
        if (data)
        {
            D3D11_SUBRESOURCE_DATA bufferData;
            ZeroMemory(&bufferData, sizeof(bufferData));
            bufferData.pSysMem = data;
            HRESULT hr = device->CreateBuffer(&bd, &bufferData, &m_Buffer);
        }
        else
        {
            HRESULT hr = device->CreateBuffer(&bd, nullptr, &m_Buffer);
        }

        if (FAILED(hr))
        {
            _com_error error(hr);
            std::string errorMessage = std::string("Failed to create vertex buffer: ") + std::string(error.ErrorMessage());
            NX_ERROR(errorMessage);
        }
    }

    UniformBufferDX11::~UniformBufferDX11()
    {
        }

    void UniformBufferDX11::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
        m_Context->Map(m_Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

        unsigned char *buffer = reinterpret_cast<unsigned char *>(mappedResource.pData);
        buffer += offset;

        memcpy(buffer, data, size);

        m_Context->Unmap(m_Buffer.Get(), 0);
    }

    Microsoft::WRL::ComPtr<ID3D11Buffer> UniformBufferDX11::GetHandle()
    {
        return m_Buffer;
    }
}

#endif