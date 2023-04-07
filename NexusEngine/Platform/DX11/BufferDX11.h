#pragma once

#include "DX11.h"
#include "Core/Graphics/Buffer.h"

namespace Nexus
{
    class VertexBufferDX11 : public VertexBuffer
    {
        public:
            VertexBufferDX11(ID3D11Device* device, const std::vector<float>& vertices);
            virtual unsigned int GetVertexCount() override { return m_VertexCount; }
            ID3D11Buffer* GetNativeHandle() { return m_VertexBuffer; }
        private:
            ID3D11Buffer* m_VertexBuffer;
            unsigned int m_VertexCount = 0;
    };

    class IndexBufferDX11 : public IndexBuffer
    {
        public:
            IndexBufferDX11(ID3D11Device* device, const std::vector<unsigned int>& indices);            
            virtual unsigned int GetIndexCount() override { return m_IndexCount; }
            ID3D11Buffer* GetNativeHandle() { return m_IndexBuffer; }        
        private:
            ID3D11Buffer* m_IndexBuffer;
            unsigned int m_IndexCount = 0;
    };
}