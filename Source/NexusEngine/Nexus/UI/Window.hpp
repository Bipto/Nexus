#pragma once

#include "Nexus/Graphics/Rectangle.hpp"
#include "Nexus/Graphics/Circle.hpp"

#include "Nexus/Utils/Utils.hpp"
#include "Nexus/Events/EventHandler.hpp"
#include "Nexus/Renderer/BatchRenderer.hpp"

#include "Nexus/Graphics/Font.hpp"
#include "Nexus/Input/InputState.hpp"

namespace Nexus::UI
{
    struct WindowClosedEvent
    {
    };

    class Window
    {
    public:
        Window(int x, int y, int width, int height, const std::string &title, Nexus::Graphics::Font *font, InputState *inputState);
        void Render(Nexus::Graphics::BatchRenderer *batchRenderer, Nexus::Graphics::RenderTarget target);
        void Update();
        bool IsClosing();

    private:
        Nexus::Graphics::Rectangle GetTitlebarRectangle();
        Nexus::Graphics::Rectangle GetCloseButtonRectangle();
        Nexus::Graphics::Circle GetBottomLeftCircle();
        Nexus::Graphics::Circle GetBottomRightCircle();

    private:
        Nexus::Graphics::Rectangle m_Rectangle;
        glm::vec2 m_MinimumSize = {100.0f, 100.0f};
        std::string m_WindowTitle;
        Nexus::Graphics::Font *m_Font = nullptr;
        uint32_t m_FontSize = 24;
        glm::vec4 m_BackgroundColour = Nexus::Utils::ColorFromRGBA(84.0f, 86.0f, 85.0f, 255.0f);
        glm::vec4 m_TitlebarColour = Nexus::Utils::ColorFromRGBA(64.0f, 66.0f, 65.0f, 255.0f);
        bool m_Dragging = false;
        InputState *m_InputState = nullptr;
        bool m_Closing = false;
        bool m_ResizeLeft = false;
        bool m_ResizeRight = false;
        bool m_ResizeDragLeftVisible = false;
        bool m_ResizeDragRightVisible = false;
    };
}