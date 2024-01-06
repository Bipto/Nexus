#pragma once

#if defined(WIN32)

#include "D3D11Include.hpp"
#include "Nexus/Graphics/Texture.hpp"

namespace Nexus::Graphics
{
    class TextureD3D11 : public Texture
    {
    public:
        TextureD3D11(ID3D11Device *device, ID3D11DeviceContext *context, const TextureSpecification &spec);
        ~TextureD3D11();
        virtual ResourceHandle GetHandle() override;
        virtual void SetData(const void *data, uint32_t size) override;

        ID3D11ShaderResourceView *GetResourceView() { return m_ResourceView; }
        ID3D11SamplerState *GetSamplerState() { return m_SamplerState; }

    private:
        ID3D11Texture2D *m_Texture;
        ID3D11ShaderResourceView *m_ResourceView;
        ID3D11SamplerState *m_SamplerState;
        DXGI_FORMAT m_TextureFormat;
        ID3D11DeviceContext *m_DeviceContext;
        uint32_t m_RowPitch = 0;
    };
}

#endif