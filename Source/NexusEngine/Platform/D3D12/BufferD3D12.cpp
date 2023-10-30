#include "BufferD3D12.hpp"

namespace Nexus::Graphics
{
    VertexBufferD3D12::VertexBufferD3D12(GraphicsDeviceD3D12 *device, const BufferDescription &description, const void *data, const VertexBufferLayout &layout)
        : VertexBuffer(description, data, layout), m_Device(device)
    {
        auto d3d12Device = device->GetDevice();
        auto d3d12CommandList = device->GetUploadCommandList();

        D3D12_HEAP_PROPERTIES uploadProperties;
        uploadProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
        uploadProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        uploadProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        uploadProperties.CreationNodeMask = 0;
        uploadProperties.VisibleNodeMask = 0;

        D3D12_RESOURCE_DESC uploadBufferDesc;
        uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        uploadBufferDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        uploadBufferDesc.Width = m_Description.Size;
        uploadBufferDesc.Height = 1;
        uploadBufferDesc.DepthOrArraySize = 1;
        uploadBufferDesc.MipLevels = 1;
        uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
        uploadBufferDesc.SampleDesc.Count = 1;
        uploadBufferDesc.SampleDesc.Quality = 0;
        uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        Microsoft::WRL::ComPtr<ID3D12Resource2> uploadBuffer;
        d3d12Device->CreateCommittedResource(&uploadProperties, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&uploadBuffer));

        D3D12_HEAP_PROPERTIES bufferProperties;
        bufferProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
        bufferProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        bufferProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        bufferProperties.CreationNodeMask = 0;
        bufferProperties.VisibleNodeMask = 0;

        D3D12_RESOURCE_DESC bufferDesc;
        bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufferDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        bufferDesc.Width = m_Description.Size;
        bufferDesc.Height = 1;
        bufferDesc.DepthOrArraySize = 1;
        bufferDesc.MipLevels = 1;
        bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
        bufferDesc.SampleDesc.Count = 1;
        bufferDesc.SampleDesc.Quality = 0;
        bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
        d3d12Device->CreateCommittedResource(&bufferProperties, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_Buffer));

        void *uploadBufferAddress;
        D3D12_RANGE uploadRange;
        uploadRange.Begin = 0;
        uploadRange.End = m_Description.Size;
        uploadBuffer->Map(0, &uploadRange, &uploadBufferAddress);
        memcpy(uploadBufferAddress, data, m_Description.Size);
        uploadBuffer->Unmap(0, &uploadRange);

        device->ImmediateSubmit([&](ID3D12GraphicsCommandList7 *cmd)
                                { cmd->CopyBufferRegion(m_Buffer.Get(), 0, uploadBuffer.Get(), 0, m_Description.Size); });

        m_VertexBufferView.BufferLocation = m_Buffer->GetGPUVirtualAddress();
        m_VertexBufferView.SizeInBytes = m_Description.Size;
        m_VertexBufferView.StrideInBytes = layout.GetStride();
    }

    VertexBufferD3D12::~VertexBufferD3D12()
    {
    }

    void VertexBufferD3D12::SetData(const void *data, uint32_t size, uint32_t offset)
    {
    }

    const D3D12_VERTEX_BUFFER_VIEW VertexBufferD3D12::GetVertexBufferView() const
    {
        return m_VertexBufferView;
    }
}