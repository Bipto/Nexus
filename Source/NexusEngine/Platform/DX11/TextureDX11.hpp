#pragma once

#if defined(WIN32)

#include "DX11.hpp"
#include "Nexus/Graphics/Texture.hpp"

namespace Nexus::Graphics
{
    class TextureDX11 : public Texture
    {
    public:
        TextureDX11(ID3D11Device *device, const TextureSpecification &spec);
        ~TextureDX11();
        virtual ResourceHandle GetHandle() override;

        ID3D11ShaderResourceView *GetResourceView() { return m_ResourceView; }
        ID3D11SamplerState *GetSamplerState() { return m_SamplerState; }

    private:
        ID3D11Texture2D *m_Texture;
        ID3D11ShaderResourceView *m_ResourceView;
        ID3D11SamplerState *m_SamplerState;
    };
}

#endif