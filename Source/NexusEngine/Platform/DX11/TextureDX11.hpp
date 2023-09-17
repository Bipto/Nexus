#pragma once

#if defined(WIN32)

#include "DX11.hpp"
#include "Core/Graphics/Texture.hpp"

namespace Nexus::Graphics
{
    class TextureDX11 : public Texture
    {
    public:
        TextureDX11(Microsoft::WRL::ComPtr<ID3D11Device> device, const TextureSpecification &spec);
        ~TextureDX11();
        virtual void *GetHandle() override;

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetResourceView() { return m_ResourceView; }
        Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSamplerState() { return m_SamplerState; }

    private:
        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_Texture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ResourceView;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;
    };
}

#endif