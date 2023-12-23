#pragma once

#include "Control.hpp"
#include "Nexus/Graphics/Rectangle.hpp"

#include "Nexus/Utils/Utils.hpp"

namespace Nexus::UI
{
    class Window : public Control
    {
    public:
        Window(int x, int y, int width, int height, const std::string &title, Nexus::Graphics::Font *font);
        virtual void Render(Nexus::Graphics::BatchRenderer *batchRenderer, Nexus::Graphics::RenderTarget target) override;
        virtual void Update() override;

    private:
        Nexus::Graphics::Rectangle GetTitlebarRectangle();
        Nexus::Graphics::Rectangle GetCloseButtonRectangle();

    private:
        Nexus::Graphics::Rectangle m_Rectangle;
        std::string m_WindowTitle;
        Nexus::Graphics::Font *m_Font = nullptr;
        uint32_t m_FontSize = 24;
        glm::vec4 m_BackgroundColour = Nexus::Utils::ColorFromRGBA(84.0f, 86.0f, 85.0f, 255.0f);
        glm::vec4 m_TitlebarColour = Nexus::Utils::ColorFromRGBA(64.0f, 66.0f, 65.0f, 255.0f);
    };
}