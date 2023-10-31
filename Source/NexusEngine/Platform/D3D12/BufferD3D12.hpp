#pragma once

#include "Nexus/Graphics/Buffer.hpp"
#include "D3D12Include.hpp"
#include "GraphicsDeviceD3D12.hpp"

namespace Nexus::Graphics
{
    class VertexBufferD3D12 : public VertexBuffer
    {
    public:
        VertexBufferD3D12(GraphicsDeviceD3D12 *device, const BufferDescription &description, const void *data, const VertexBufferLayout &layout);
        virtual ~VertexBufferD3D12();
        virtual void SetData(const void *data, uint32_t size, uint32_t offset) override;
        const D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource2> m_VertexBuffer = nullptr;
        Microsoft::WRL::ComPtr<ID3D12Resource2> m_UploadBuffer = nullptr;
        D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
        GraphicsDeviceD3D12 *m_Device = nullptr;
    };

    class IndexBufferD3D12 : public IndexBuffer
    {
    public:
        IndexBufferD3D12(GraphicsDeviceD3D12 *device, const BufferDescription &description, const void *data, IndexBufferFormat format);
        virtual ~IndexBufferD3D12();
        virtual void SetData(const void *data, uint32_t size, uint32_t offset) override;
        void CreateIndexBufferFormat();
        const D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource2> m_IndexBuffer = nullptr;
        Microsoft::WRL::ComPtr<ID3D12Resource2> m_UploadBuffer = nullptr;
        D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
        DXGI_FORMAT m_IndexBufferFormat;
        GraphicsDeviceD3D12 *m_Device = nullptr;
    };
}