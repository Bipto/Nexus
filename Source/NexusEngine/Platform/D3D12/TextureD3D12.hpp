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
        virtual ResourceHandle GetHandle() override;
        virtual void SetData(const void *data, uint32_t size) override;
        DXGI_FORMAT GetFormat();
        ID3D12Resource2 *GetD3D12ResourceHandle();

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource2> m_Texture = nullptr;
        Microsoft::WRL::ComPtr<ID3D12Resource2> m_UploadBuffer = nullptr;

        DXGI_FORMAT m_TextureFormat = DXGI_FORMAT_UNKNOWN;

        TextureSpecification m_Specification;
        GraphicsDeviceD3D12 *m_Device = nullptr;
    };
}

#endif