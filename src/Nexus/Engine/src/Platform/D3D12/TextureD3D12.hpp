#pragma once

#if defined(NX_PLATFORM_D3D12)

#include "D3D12Include.hpp"
#include "GraphicsDeviceD3D12.hpp"
#include "RHI/Texture.hpp"

namespace Nexus::Graphics
{
    class TextureD3D12 : public ITexture
    {
      public:
        TextureD3D12(const TextureDescription &spec, GraphicsDeviceD3D12 *device);
        TextureD3D12(
            Microsoft::WRL::ComPtr<ID3D12Resource2> handle,
            const TextureDescription &spec, GraphicsDeviceD3D12 *device
        );
        virtual ~TextureD3D12();

        TextureLayout GetTextureLayout(
            uint32_t arrayLayer, uint32_t mipLevel
        ) const final;
        void SetTextureLayout(
            uint32_t arrayLayer, uint32_t mipLevel, TextureLayout layout
        );
        SubresourceFootprint GetSubresourceFootprint(
            uint32_t arrayLayer, uint32_t mipLevel
        ) const final;

        const DXGI_FORMAT GetFormat() const;

        const Microsoft::WRL::ComPtr<ID3D12Resource2> GetHandle() const;

        /// @brief This is a function to force all ComPtr handles to be released,
        /// used when resizing a swapchain
        /// @param waitForIdle A boolean value indicating whether the handle should
        /// be released after the device is idle or immediately
        void ReleaseHandle(bool waitForIdle);

      private:
        Microsoft::WRL::ComPtr<ID3D12Resource2> m_Texture = nullptr;
        Microsoft::WRL::ComPtr<D3D12MA::Allocation> m_Allocation = nullptr;
        DXGI_FORMAT m_TextureFormat = DXGI_FORMAT_UNKNOWN;

        GraphicsDeviceD3D12 *m_Device = nullptr;

        D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceView = {};

        std::vector<TextureLayout> m_TextureLayout = {};
    };
} // namespace Nexus::Graphics

#endif