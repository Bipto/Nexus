#pragma once

#include "Control.hpp"

namespace Nexus::UI
{
    class ImageButton : public Control
    {
    public:
        virtual void Render(Graphics::BatchRenderer *batchRenderer) override;

        void SetTexture(const Ref<Graphics::Texture> texture);
        const Ref<Graphics::Texture> GetTexture() const;

        void SetFilter(const glm::vec4 &filter);
        const glm::vec4 &GetFilter() const;

    private:
        Ref<Graphics::Texture> m_Texture = nullptr;
        glm::vec4 m_Filter = {1.0f, 1.0f, 1.0f, 1.0f};
        glm::vec4 m_HoveredColour = {0.0f, 0.67f, 0.94f, 1.0f};
        glm::vec4 m_ClickedColour = {0.0f, 0.57f, 0.84f, 1.0f};
    };
}