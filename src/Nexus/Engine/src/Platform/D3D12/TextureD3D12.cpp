#include "TextureD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

#include "D3D12Utils.hpp"
#include "Nexus-Core/Utils/Utils.hpp"

namespace Nexus::Graphics
{
    TextureD3D12::TextureD3D12(const TextureDescription &spec, GraphicsDeviceD3D12 *device)
        : ITexture(spec), m_Device(device)
    {
        NX_VALIDATE(spec.DepthOrArrayLayers >= 1, "Texture must have at least one array layer");
        NX_VALIDATE(spec.DepthOrArrayLayers >= 1, "Texture must have at least one mip level");

        if (spec.Samples > 1)
        {
            NX_VALIDATE(spec.MipLevels == 1, "Multisampled textures do not support mipmapping");
        }

        if (spec.Type == TextureType::TextureCube)
        {
            NX_VALIDATE(spec.DepthOrArrayLayers % 6 == 0, "Cubemap textures must have a multiple of 6 faces");
        }

        D3D12_RESOURCE_DIMENSION dimension = D3D12::GetResourceDimensions(spec.Type);
        D3D12_RESOURCE_FLAGS flags = D3D12::GetResourceFlags(spec);
        m_TextureFormat = D3D12::GetD3D12PixelFormat(spec.Format);

        D3D12_RESOURCE_DESC textureDesc = {};
        textureDesc.Dimension = dimension;
        textureDesc.Alignment = 0;
        textureDesc.Width = spec.Width;
        textureDesc.Height = spec.Height;
        textureDesc.DepthOrArraySize = spec.DepthOrArrayLayers;
        textureDesc.MipLevels = spec.MipLevels;
        textureDesc.Format = m_TextureFormat;
        textureDesc.SampleDesc.Count = spec.Samples;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Flags = flags;

        HRESULT hr = {};

        if (m_Description.CreateFlags & Graphics::TextureCreateFlags_SparseBinding)
        {
            textureDesc.Layout = D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE;

            Microsoft::WRL::ComPtr<ID3D12Device9> device = m_Device->GetD3D12Device();
            hr = device->CreateReservedResource(
                &textureDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_Texture)
            );
        }
        else
        {
            textureDesc.Layout = D3D12::GetTextureLayout(spec.Tiling);

            D3D12MA::ALLOCATION_DESC allocationDesc = {};
            allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
            allocationDesc.Flags = D3D12MA::ALLOCATION_FLAG_COMMITTED;

            Microsoft::WRL::ComPtr<D3D12MA::Allocator> allocator = device->GetAllocator();
            hr = allocator->CreateResource(
                &allocationDesc, &textureDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, &m_Allocation,
                IID_PPV_ARGS(&m_Texture)
            );
        }

        if (FAILED(hr))
        {
            _com_error error(hr);
            std::string errorMessage = std::string("Failed to create texture: ") + error.ErrorMessage();
            throw std::runtime_error(errorMessage.c_str());
        }

        std::wstring name = std::wstring(m_Description.DebugName.begin(), m_Description.DebugName.end());
        m_Texture->SetName(name.c_str());

        m_TextureLayout.resize(spec.DepthOrArrayLayers * spec.MipLevels, TextureLayout::Undefined);
    }

    TextureD3D12::TextureD3D12(
        Microsoft::WRL::ComPtr<ID3D12Resource2> handle, const TextureDescription &spec, GraphicsDeviceD3D12 *device
    )
        : m_Texture(handle), m_Device(device), ITexture(spec)
    {
        NX_VALIDATE(spec.DepthOrArrayLayers >= 1, "Texture must have at least one array layer");
        NX_VALIDATE(spec.DepthOrArrayLayers >= 1, "Texture must have at least one mip level");

        if (spec.Samples > 1)
        {
            NX_VALIDATE(spec.MipLevels == 1, "Multisampled textures do not support mipmapping");
        }

        if (spec.Type == TextureType::TextureCube)
        {
            NX_VALIDATE(spec.DepthOrArrayLayers % 6 == 0, "Cubemap textures must have a multiple of 6 faces");
        }

        std::wstring name = std::wstring(m_Description.DebugName.begin(), m_Description.DebugName.end());
        m_Texture->SetName(name.c_str());

        m_TextureLayout.resize(spec.DepthOrArrayLayers * spec.MipLevels, TextureLayout::Undefined);
    }

    TextureD3D12::~TextureD3D12()
    {
        ReleaseHandle(true);
    }

    TextureLayout TextureD3D12::GetTextureLayout(uint32_t arrayLayer, uint32_t mipLevel) const
    {
        NX_VALIDATE(arrayLayer < m_Description.DepthOrArrayLayers, "Array layer out of bounds");
        NX_VALIDATE(mipLevel < m_Description.MipLevels, "Mip level out of bounds");

        size_t index = (size_t)(mipLevel + arrayLayer * m_Description.MipLevels);
        return m_TextureLayout[index];
    }

    void TextureD3D12::SetTextureLayout(uint32_t arrayLayer, uint32_t mipLevel, TextureLayout layout)
    {
        NX_VALIDATE(arrayLayer < m_Description.DepthOrArrayLayers, "Array layer out of bounds");
        NX_VALIDATE(mipLevel < m_Description.MipLevels, "Mip level out of bounds");

        size_t index = (size_t)(mipLevel + arrayLayer * m_Description.MipLevels);
        m_TextureLayout[index] = layout;
    }

    SubresourceFootprint TextureD3D12::GetSubresourceFootprint(uint32_t arrayLayer, uint32_t mipLevel) const
    {
        Microsoft::WRL::ComPtr<ID3D12Device9> device = m_Device->GetD3D12Device();
        uint32_t subresourceIndex = Utils::CalculateSubresource(mipLevel, arrayLayer, m_Description.MipLevels);

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedFootprint = {};
        UINT numRows = {};
        UINT64 rowSizeInBytes = {};
        UINT64 totalBytes = {};

        D3D12_RESOURCE_DESC resourceDesc = m_Texture->GetDesc();
        device->GetCopyableFootprints(
            &resourceDesc, subresourceIndex, 1, 0, &placedFootprint, &numRows, &rowSizeInBytes, &totalBytes
        );

        SubresourceFootprint footprint = {};
        footprint.Size = totalBytes;
        footprint.RowPitch = placedFootprint.Footprint.RowPitch;
        footprint.RowCount = numRows;

        return footprint;
    }

    const DXGI_FORMAT TextureD3D12::GetFormat() const
    {
        return m_TextureFormat;
    }

    const Microsoft::WRL::ComPtr<ID3D12Resource2> TextureD3D12::GetHandle() const
    {
        return m_Texture;
    }

    void TextureD3D12::ReleaseHandle(bool waitForIdle)
    {
        // Optionally ensure the device is idle before releasing GPU resources.
        if (waitForIdle && m_Device)
        {
            m_Device->WaitForIdle();
        }

        // Clear CPU-side state
        m_TextureLayout.clear();
        shaderResourceView = {};

        // Release COM/resource references owned by this object.
        if (m_Texture)
        {
            m_Texture.Reset();
        }

        if (m_Allocation)
        {
            m_Allocation.Reset();
        }

        // Reset format to unknown
        m_TextureFormat = DXGI_FORMAT_UNKNOWN;
    }
} // namespace Nexus::Graphics

#endif