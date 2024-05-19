#pragma once

#include "Control.hpp"

#include <string>

namespace Nexus::UI
{
    class Button : public Control
    {
    public:
        virtual void Render(Graphics::BatchRenderer *batchRenderer) override;

        void SetText(const std::string &text);
        const std::string &GetText() const;

        void SetBorderThickness(uint32_t thickness);
        const uint32_t GetBorderThickness() const;

    private:
        std::string m_Text;
        uint32_t m_BorderThickness = 1;
        glm::vec4 m_HoveredColour = {0.0f, 0.67f, 0.94f, 1.0f};
        glm::vec4 m_ClickedColour = {0.0f, 0.57f, 0.84f, 1.0f};
    };
}