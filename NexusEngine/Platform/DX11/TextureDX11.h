#pragma once

#include "DX11.h"
#include "Core/Graphics/Texture.h"

namespace Nexus
{
    class TextureDX11 : public Texture
    {
        public:
            TextureDX11(ID3D11Device* device, const char* filepath);
            ~TextureDX11();
            virtual void Bind(unsigned int slot = 0) override;
            virtual void* GetHandle() override;
        private:
            ID3D11Texture2D* m_Texture;
            ID3D11ShaderResourceView* m_ResourceView;
    };
}