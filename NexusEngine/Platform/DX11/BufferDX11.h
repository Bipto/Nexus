#pragma once

#include "DX11.h"
#include "Core/Graphics/Buffer.h"

namespace Nexus
{
    class VertexBufferDX11 : public VertexBuffer
    {
        public:
            VertexBufferDX11(ID3D11Device* device, const std::vector<float>& vertices)
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
                    std::cout << "Failed to create vertex buffer\n";
                }
                else
                {
                    std::cout << "Successfully created vertex buffer\n";
                }
            }

            virtual unsigned int GetVertexCount() override { return m_VertexCount; }

            ID3D11Buffer* GetNativeHandle() { return m_VertexBuffer; }
        
        private:
            ID3D11Buffer* m_VertexBuffer;
            unsigned int m_VertexCount = 0;
    };
}