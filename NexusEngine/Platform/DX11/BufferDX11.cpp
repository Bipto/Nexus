#if defined(WIN32)

#include "BufferDX11.hpp"
#include "Core/Logging/Log.hpp"

namespace Nexus::Graphics
{
    D3D11_BIND_FLAG GetBufferType(BufferType type)
    {
        switch (type)
        {
        case BufferType::Vertex:
            return D3D11_BIND_VERTEX_BUFFER;
        case BufferType::Index:
            return D3D11_BIND_INDEX_BUFFER;
        case BufferType::Uniform:
            return D3D11_BIND_CONSTANT_BUFFER;
        default:
            throw std::runtime_error("Invalid buffer type entered");
        }
    }

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

    DeviceBufferDX11::DeviceBufferDX11(ID3D11Device *device, ID3D11DeviceContext *context, const BufferDescription &description, const void *data)
        : DeviceBuffer(description, data)
    {
        m_Context = context;

        auto bufferType = GetBufferType(m_Description.Type);
        auto bufferUsage = GetBufferUsage(m_Description.Usage);

        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));

        bd.Usage = bufferUsage;
        bd.ByteWidth = m_Description.Size;
        bd.BindFlags = bufferType;

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

    void DeviceBufferDX11::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        if (m_Description.Usage == BufferUsage::Static)
        {
            throw std::runtime_error("Attempting to upload data to a static buffer");
            return;
        }

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

        m_Context->Map(m_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        memcpy(mappedResource.pData, data, size);
        m_Context->Unmap(m_Buffer, 0);
    }
}

#endif