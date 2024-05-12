#pragma once

#include "Nexus/Renderer/BatchRenderer.hpp"

#include "Nexus/Point.hpp"

#include "Nexus/Graphics/Font.hpp"

#include "Nexus/Events/EventHandler.hpp"

#include "Canvas.hpp"

#include <glm/glm.hpp>

namespace Nexus::UI
{
    class Control
    {
    public:
        virtual void Update();
        virtual void Render(Graphics::BatchRenderer *batchRenderer);

        void SetPosition(const Point<int> &position);
        void SetSize(const Point<int> &size);
        void SetBackgroundColour(const glm::vec4 &colour);
        void SetForegroundColour(const glm::vec4 &colour);
        void SetFont(Graphics::Font *font);
        void SetFontSize(uint32_t fontSize);
        void SetMarginTop(uint32_t margin);
        void SetMarginBottom(uint32_t margin);
        void SetMarginLeft(uint32_t margin);
        void SetMarginRight(uint32_t margin);
        void SetMargin(uint32_t left, uint32_t right, uint32_t top, uint32_t bottom);

        const Point<int> &GetPosition() const;
        const Point<int> &GetSize() const;
        const glm::vec4 &GetBackgroundColour() const;
        const glm::vec4 &GetForegroundColour() const;
        const Graphics::Font *GetFont() const;
        const uint32_t GetFontSize() const;
        const uint32_t GetMarginTop() const;
        const uint32_t GetMarginBottom() const;
        const uint32_t GetMarginLeft() const;
        const uint32_t GetMarginRight() const;

        const Nexus::Graphics::Rectangle<float> GetRectangle() const;
        const Canvas *GetCanvas() const;

        EventHandler<Control *> OnClick;
        EventHandler<Control *> OnMouseEnter;
        EventHandler<Control *> OnMouseLeave;

    protected:
        Point<int> m_Position = {0, 0};
        Point<int> m_Size = {0, 0};
        uint32_t m_FontSize = 24;
        glm::vec4 m_BackgroundColour = {1, 1, 1, 1};
        glm::vec4 m_ForegroundColour = {0, 0, 0, 1};
        Graphics::Font *m_Font = nullptr;

        uint32_t m_MarginLeft = 0;
        uint32_t m_MarginTop = 0;
        uint32_t m_MarginRight = 0;
        uint32_t m_MarginBottom = 0;

        bool m_Hovered = false;

    private:
        Canvas *m_Canvas = nullptr;
        friend class Canvas;
    };
}