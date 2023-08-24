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

    D3D11_MAP GetMapMode(MapMode mode)
    {
        switch (mode)
        {
        case MapMode::Read:
            return D3D11_MAP_READ;
        case MapMode::Write:
            return D3D11_MAP_WRITE_DISCARD;
        case MapMode::ReadWrite:
            return D3D11_MAP_READ_WRITE;
        default:
            throw std::runtime_error("Invalid map mode entered");
        }
    }

    VertexBufferDX11::VertexBufferDX11(ID3D11Device *device, ID3D11DeviceContext *context, const BufferDescription &description, const void *data, const VertexBufferLayout &layout)
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
        m_Buffer->Release();
        m_Buffer = nullptr;
    }

    void *VertexBufferDX11::Map(MapMode mode)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
        m_Context->Map(m_Buffer, 0, GetMapMode(mode), 0, &mappedResource);
        return mappedResource.pData;
    }

    void VertexBufferDX11::Unmap()
    {
        m_Context->Unmap(m_Buffer, 0);
    }

    ID3D11Buffer *VertexBufferDX11::GetHandle()
    {
        return m_Buffer;
    }

    IndexBufferDX11::IndexBufferDX11(ID3D11Device *device, ID3D11DeviceContext *context, const BufferDescription &description, const void *data)
        : IndexBuffer(description, data)
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
        m_Buffer->Release();
        m_Buffer = nullptr;
    }

    void *IndexBufferDX11::Map(MapMode mode)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
        m_Context->Map(m_Buffer, 0, GetMapMode(mode), 0, &mappedResource);
        return mappedResource.pData;
    }

    void IndexBufferDX11::Unmap()
    {
        m_Context->Unmap(m_Buffer, 0);
    }

    ID3D11Buffer *IndexBufferDX11::GetHandle()
    {
        return m_Buffer;
    }

    UniformBufferDX11::UniformBufferDX11(ID3D11Device *device, ID3D11DeviceContext *context, const BufferDescription &description, const void *data)
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
        m_Buffer->Release();
        m_Buffer = nullptr;
    }

    void *UniformBufferDX11::Map(MapMode mode)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
        m_Context->Map(m_Buffer, 0, GetMapMode(mode), 0, &mappedResource);
        return mappedResource.pData;
    }

    void UniformBufferDX11::Unmap()
    {
        m_Context->Unmap(m_Buffer, 0);
    }

    ID3D11Buffer *UniformBufferDX11::GetHandle()
    {
        return m_Buffer;
    }
}

#endif