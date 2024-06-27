#pragma once

#include "Nexus/Graphics/Texture.hpp"
#include "Control.hpp"

namespace Nexus::UI
{
    class PictureBox : public Control
    {
    public:
        virtual void OnUpdate() override;
        virtual void OnRender(Nexus::Graphics::BatchRenderer *renderer) override;
        virtual void OnAutoSize() override;
        virtual void HandleMouseClick(const MouseClick &e) override;

        void SetTexture(Ref<Graphics::Texture> texture);
        const Ref<Graphics::Texture> GetTexture() const;

    private:
        Ref<Graphics::Texture> m_Texture = nullptr;
    };
}