#pragma once

#if defined(WIN32)

#include "DX11.h"
#include "Core/Graphics/Texture.h"

namespace Nexus
{
    class TextureDX11 : public Texture
    {
    public:
        TextureDX11(ID3D11Device *device, TextureSpecification spec);
        ~TextureDX11();
        virtual void *GetHandle() override;

        const ID3D11ShaderResourceView *const GetResourceView() const { return m_ResourceView; }
        const ID3D11SamplerState *const GetSamplerState() const { return m_SamplerState; }

    private:
        ID3D11Texture2D *m_Texture;
        ID3D11ShaderResourceView *m_ResourceView;
        ID3D11SamplerState *m_SamplerState;
    };
}

#endif