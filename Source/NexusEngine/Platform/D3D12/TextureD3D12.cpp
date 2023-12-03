#include "TextureD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

#include "D3D12Utils.hpp"

namespace Nexus::Graphics
{
    TextureD3D12::TextureD3D12(GraphicsDeviceD3D12 *device, const TextureSpecification &spec)
        : Texture(spec), m_Specification(spec), m_Device(device)
    {
        auto d3d12Device = device->GetDevice();
        m_TextureFormat = GetD3D12TextureFormat(spec.Format);

        D3D12_HEAP_PROPERTIES uploadProperties;
        uploadProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
        uploadProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        uploadProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        uploadProperties.CreationNodeMask = 0;
        uploadProperties.VisibleNodeMask = 0;

        D3D12_RESOURCE_DESC uploadBufferDesc;
        uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        uploadBufferDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        uploadBufferDesc.Width = spec.Width * spec.Height * spec.NumberOfChannels;
        uploadBufferDesc.Height = 1;
        uploadBufferDesc.DepthOrArraySize = 1;
        uploadBufferDesc.MipLevels = 1;
        uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
        uploadBufferDesc.SampleDesc.Count = 1;
        uploadBufferDesc.SampleDesc.Quality = 0;
        uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
        d3d12Device->CreateCommittedResource(&uploadProperties, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_UploadBuffer));

        D3D12_HEAP_PROPERTIES heapProperties;
        heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProperties.CreationNodeMask = 0;
        heapProperties.VisibleNodeMask = 0;

        D3D12_RESOURCE_DESC resourceDesc;
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        resourceDesc.Width = spec.Width;
        resourceDesc.Height = spec.Height;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = m_TextureFormat;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
        d3d12Device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_Texture));

        SetData(spec.Data);
    }

    TextureD3D12::~TextureD3D12()
    {
    }

    ResourceHandle TextureD3D12::GetHandle()
    {
        return ResourceHandle();
    }

    DXGI_FORMAT TextureD3D12::GetFormat()
    {
        return m_TextureFormat;
    }

    ID3D12Resource2 *TextureD3D12::GetD3D12ResourceHandle()
    {
        return m_Texture.Get();
    }

    void TextureD3D12::SetData(const void *data)
    {
        uint32_t size = m_Specification.Width * m_Specification.Height * m_Specification.NumberOfChannels;
        uint32_t stride = m_Specification.Width * m_Specification.NumberOfChannels;

        void *uploadBufferAddress;
        D3D12_RANGE uploadRange;
        uploadRange.Begin = 0;
        uploadRange.End = size;

        m_UploadBuffer->Map(0, &uploadRange, &uploadBufferAddress);
        memcpy(uploadBufferAddress, data, size);
        m_UploadBuffer->Unmap(0, &uploadRange);

        m_Device->ImmediateSubmit([&](ID3D12GraphicsCommandList7 *cmd)
                                  {
                                      D3D12_BOX textureSizeAsBox;
                                      textureSizeAsBox.left = 0;
                                      textureSizeAsBox.top = 0;
                                      textureSizeAsBox.front = 0;
                                      textureSizeAsBox.right = m_Specification.Width;
                                      textureSizeAsBox.bottom = m_Specification.Height;
                                      textureSizeAsBox.back = 1;

                                      D3D12_TEXTURE_COPY_LOCATION textureSource, textureDestination;
                                      textureSource.pResource = m_UploadBuffer.Get();
                                      textureSource.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
                                      textureSource.PlacedFootprint.Offset = 0;
                                      textureSource.PlacedFootprint.Footprint.Width = m_Specification.Width;
                                      textureSource.PlacedFootprint.Footprint.Height = m_Specification.Height;
                                      textureSource.PlacedFootprint.Footprint.Depth = 1;
                                      textureSource.PlacedFootprint.Footprint.RowPitch = stride;
                                      textureSource.PlacedFootprint.Footprint.Format = m_TextureFormat;

                                      textureDestination.pResource = m_Texture.Get();
                                      textureDestination.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
                                      textureDestination.SubresourceIndex = 0;
                                      cmd->CopyTextureRegion(&textureDestination, 0, 0, 0, &textureSource, &textureSizeAsBox); });
    }
}

#endif