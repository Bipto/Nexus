#include "TextureD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

#include "D3D12Utils.hpp"

namespace Nexus::Graphics
{
TextureD3D12::TextureD3D12(GraphicsDeviceD3D12 *device, const TextureSpecification &spec) : Texture(spec, device), m_Specification(spec), m_Device(device)
{
    bool isDepth;
    D3D12_RESOURCE_FLAGS flags = D3D12::GetD3D12ResourceFlags(spec.Usage, isDepth);
    ID3D12Device10 *d3d12Device = device->GetDevice();
    m_TextureFormat = D3D12::GetD3D12PixelFormat(spec.Format, isDepth);

    const D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;

    uint32_t samples = GetSampleCount(spec.Samples);

    // staging buffer
    {
        D3D12_HEAP_PROPERTIES uploadProperties;
        uploadProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
        uploadProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        uploadProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        uploadProperties.CreationNodeMask = 0;
        uploadProperties.VisibleNodeMask = 0;

        uint32_t sizeInBytes = GetPixelFormatSizeInBytes(m_Specification.Format);

        D3D12_RESOURCE_DESC uploadBufferDesc;
        uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        uploadBufferDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        uploadBufferDesc.Width = spec.Width * spec.Height * sizeInBytes;
        uploadBufferDesc.Height = 1;
        uploadBufferDesc.DepthOrArraySize = 1;
        uploadBufferDesc.MipLevels = 1;
        uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
        uploadBufferDesc.SampleDesc.Count = 1;
        uploadBufferDesc.SampleDesc.Quality = 0;
        uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
        d3d12Device->CreateCommittedResource(&uploadProperties, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc, initialState, nullptr, IID_PPV_ARGS(&m_UploadBuffer));
    }

    // texture
    {
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
        resourceDesc.MipLevels = m_Specification.Levels;
        resourceDesc.Format = m_TextureFormat;
        resourceDesc.SampleDesc.Count = samples;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        resourceDesc.Flags = flags;
        d3d12Device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, initialState, nullptr, IID_PPV_ARGS(&m_Texture));
    }

    for (uint32_t level = 0; level < spec.Levels; level++)
    {
        m_ResourceStates.push_back(D3D12_RESOURCE_STATE_COMMON);
    }
}

TextureD3D12::~TextureD3D12()
{
}

void TextureD3D12::SetData(const void *data, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    uint32_t stride = width * GetPixelFormatSizeInBytes(m_Specification.Format);
    uint32_t size = width * height * GetPixelFormatSizeInBytes(m_Specification.Format);

    void *uploadBufferAddress;
    D3D12_RANGE uploadRange;
    uploadRange.Begin = 0;
    uploadRange.End = size;

    m_UploadBuffer->Map(0, nullptr, &uploadBufferAddress);
    memcpy(uploadBufferAddress, data, size);
    m_UploadBuffer->Unmap(0, &uploadRange);

    D3D12_RESOURCE_STATES resourceState = GetResourceState(level);

    D3D12_RESOURCE_BARRIER toDestBarrier = {};
    toDestBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    toDestBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    toDestBarrier.Transition.pResource = m_Texture.Get();
    toDestBarrier.Transition.Subresource = level;
    toDestBarrier.Transition.StateBefore = resourceState;
    toDestBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;

    D3D12_RESOURCE_BARRIER toDefaultBarrier = {};
    toDefaultBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    toDefaultBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    toDefaultBarrier.Transition.pResource = m_Texture.Get();
    toDefaultBarrier.Transition.Subresource = level;
    toDefaultBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    toDefaultBarrier.Transition.StateAfter = resourceState;

    m_Device->ImmediateSubmit([&](ID3D12GraphicsCommandList7 *cmd) {
        D3D12_BOX textureSizeAsBox;
        textureSizeAsBox.left = x;
        textureSizeAsBox.top = y;
        textureSizeAsBox.front = 0;
        textureSizeAsBox.right = width;
        textureSizeAsBox.bottom = height;
        textureSizeAsBox.back = 1;

        D3D12_TEXTURE_COPY_LOCATION textureSource, textureDestination;
        textureSource.pResource = m_UploadBuffer.Get();
        textureSource.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        textureSource.PlacedFootprint.Offset = 0;
        textureSource.PlacedFootprint.Footprint.Width = width;
        textureSource.PlacedFootprint.Footprint.Height = height;
        textureSource.PlacedFootprint.Footprint.Depth = 1;
        textureSource.PlacedFootprint.Footprint.RowPitch = stride;
        textureSource.PlacedFootprint.Footprint.Format = m_TextureFormat;

        textureDestination.pResource = m_Texture.Get();
        textureDestination.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        textureDestination.SubresourceIndex = level;

        cmd->ResourceBarrier(1, &toDestBarrier);
        cmd->CopyTextureRegion(&textureDestination, x, y, 0, &textureSource, &textureSizeAsBox);
        cmd->ResourceBarrier(1, &toDefaultBarrier);
    });
}

std::vector<std::byte> TextureD3D12::GetData(uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    ID3D12Device10 *d3d12Device = m_Device->GetDevice();
    D3D12_RESOURCE_DESC textureDesc = m_Texture->GetDesc();

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout;
    UINT numRows;
    UINT64 rowSizeInBytes;
    UINT64 totalBytes;
    d3d12Device->GetCopyableFootprints(&textureDesc, level, 1, 0, &layout, &numRows, &rowSizeInBytes, &totalBytes);

    D3D12_HEAP_PROPERTIES readbackProperties = {};
    readbackProperties.Type = D3D12_HEAP_TYPE_READBACK;
    readbackProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    readbackProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    readbackProperties.CreationNodeMask = 0;
    readbackProperties.VisibleNodeMask = 0;

    D3D12_RESOURCE_DESC readbackBufferDesc = {};
    readbackBufferDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    readbackBufferDesc.DepthOrArraySize = 1;
    readbackBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    readbackBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    readbackBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
    readbackBufferDesc.Height = 1;
    readbackBufferDesc.Width = totalBytes;
    readbackBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    readbackBufferDesc.MipLevels = 1;
    readbackBufferDesc.SampleDesc.Count = 1;
    readbackBufferDesc.SampleDesc.Quality = 0;

    Microsoft::WRL::ComPtr<ID3D12Resource2> readbackBuffer;

    d3d12Device->CreateCommittedResource(&readbackProperties, D3D12_HEAP_FLAG_NONE, &readbackBufferDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&readbackBuffer));

    D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
    srcLocation.pResource = m_Texture.Get();
    srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    srcLocation.SubresourceIndex = level;

    uint32_t stride = width * GetPixelFormatSizeInBytes(m_Specification.Format);

    D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
    dstLocation.pResource = readbackBuffer.Get();
    dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    dstLocation.PlacedFootprint.Offset = 0;
    dstLocation.PlacedFootprint.Footprint.Depth = 1;
    dstLocation.PlacedFootprint.Footprint.Format = m_TextureFormat;
    dstLocation.PlacedFootprint.Footprint.Width = width;
    dstLocation.PlacedFootprint.Footprint.Height = height;
    dstLocation.PlacedFootprint.Footprint.RowPitch = stride;

    D3D12_BOX textureBounds = {};
    textureBounds.left = x;
    textureBounds.right = textureBounds.left + width;
    textureBounds.top = y;
    textureBounds.bottom = textureBounds.top + height;
    textureBounds.front = 0;
    textureBounds.back = 1;

    D3D12_RESOURCE_STATES resourceState = GetResourceState(level);

    D3D12_RESOURCE_BARRIER toReadBarrier = {};
    toReadBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    toReadBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    toReadBarrier.Transition.pResource = m_Texture.Get();
    toReadBarrier.Transition.Subresource = level;
    toReadBarrier.Transition.StateBefore = resourceState;
    toReadBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;

    D3D12_RESOURCE_BARRIER toDefaultBarrier = {};
    toDefaultBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    toDefaultBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    toDefaultBarrier.Transition.pResource = m_Texture.Get();
    toDefaultBarrier.Transition.Subresource = level;
    toDefaultBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
    toDefaultBarrier.Transition.StateAfter = resourceState;

    m_Device->ImmediateSubmit([&](ID3D12GraphicsCommandList7 *cmd) {
        cmd->ResourceBarrier(1, &toReadBarrier);
        cmd->CopyTextureRegion(&dstLocation, x, y, 0, &srcLocation, &textureBounds);
        cmd->ResourceBarrier(1, &toDefaultBarrier);
    });

    std::vector<std::byte> pixels(totalBytes);

    void *uploadBufferAddress;
    D3D12_RANGE readRange;
    readRange.Begin = 0;
    readRange.End = totalBytes;

    readbackBuffer->Map(0, &readRange, &uploadBufferAddress);
    memcpy(pixels.data(), uploadBufferAddress, pixels.size());
    readbackBuffer->Unmap(0, nullptr);

    return pixels;
}

DXGI_FORMAT TextureD3D12::GetFormat()
{
    return m_TextureFormat;
}

const Microsoft::WRL::ComPtr<ID3D12Resource2> &TextureD3D12::GetD3D12ResourceHandle()
{
    return m_Texture;
}

void TextureD3D12::SetResourceState(uint32_t level, D3D12_RESOURCE_STATES state)
{
    m_ResourceStates[level] = state;
}

D3D12_RESOURCE_STATES TextureD3D12::GetResourceState(uint32_t level)
{
    return m_ResourceStates[level];
}
} // namespace Nexus::Graphics

#endif