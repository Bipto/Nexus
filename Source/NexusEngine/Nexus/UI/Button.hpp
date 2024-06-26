#pragma once

#include "Control.hpp"

#include <string>

namespace Nexus::UI
{
    class Button : public Control
    {
    public:
        virtual void OnUpdate() override;
        virtual void OnRender(Nexus::Graphics::BatchRenderer *renderer) override;

        void SetText(const std::string &text);
        void SetHoveredColour(const glm::vec4 &colour);

        const std::string &GetText() const;
        const glm::vec4 GetHoveredColour() const;

    private:
        std::string m_Text;
        glm::vec4 m_HoveredColour = {0.42f, 0.52, 0.73f, 1.0f};
    };
}