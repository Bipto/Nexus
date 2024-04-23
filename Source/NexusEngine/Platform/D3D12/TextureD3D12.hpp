#pragma once

#if defined(NX_PLATFORM_D3D12)

#include "D3D12Include.hpp"
#include "GraphicsDeviceD3D12.hpp"
#include "Nexus/Graphics/Texture.hpp"

namespace Nexus::Graphics
{
    class TextureD3D12 : public Texture
    {
    public:
        TextureD3D12(GraphicsDeviceD3D12 *device, const TextureSpecification &spec);
        virtual ~TextureD3D12();
        virtual void SetData(const void *data, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
        virtual std::vector<std::byte> GetData(uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
        DXGI_FORMAT GetFormat();
        const Microsoft::WRL::ComPtr<ID3D12Resource2> &GetD3D12ResourceHandle();

        D3D12_RESOURCE_STATES GetCurrentResourceState();
        void SetCurrentResourceState(D3D12_RESOURCE_STATES state);

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource2> m_Texture = nullptr;
        Microsoft::WRL::ComPtr<ID3D12Resource2> m_UploadBuffer = nullptr;

        DXGI_FORMAT m_TextureFormat = DXGI_FORMAT_UNKNOWN;
        D3D12_RESOURCE_STATES m_CurrentResourceState = D3D12_RESOURCE_STATE_COMMON;

        TextureSpecification m_Specification;
        GraphicsDeviceD3D12 *m_Device = nullptr;
    };
}

#endif