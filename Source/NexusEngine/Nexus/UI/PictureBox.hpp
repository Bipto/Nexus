#pragma once

#include "Control.hpp"
#include "Nexus/Graphics/Texture.hpp"

#include <string>

namespace Nexus::UI
{
    class PictureBox : public Control
    {
    public:
        virtual void Render(Graphics::BatchRenderer *batchRenderer) override;

        void SetTexture(const Nexus::Ref<Nexus::Graphics::Texture> texture);
        const Nexus::Ref<Nexus::Graphics::Texture> GetTexture() const;

    private:
        Nexus::Ref<Nexus::Graphics::Texture> m_Texture = nullptr;
    };
}