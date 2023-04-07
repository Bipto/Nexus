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
        else
        {
            NX_LOG("Vertex buffer created successfully");
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
        else
        {
            NX_LOG("Index buffer created successfully");
        }

        m_IndexCount = indices.size();
    }
}