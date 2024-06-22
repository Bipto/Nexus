#pragma once

#include "Nexus/Renderer/BatchRenderer.hpp"

#include "Nexus/Point.hpp"

#include "Nexus/Graphics/Font.hpp"

#include "Nexus/Events/EventHandler.hpp"

#include "Nexus/Input/Input.hpp"

#include "Canvas.hpp"

#include <glm/glm.hpp>

namespace Nexus::UI
{
    class Control
    {
    public:
        virtual void Update();
        virtual void Render(Graphics::BatchRenderer *batchRenderer) = 0;

        void SetPosition(const Point2D<int> &position);
        void SetSize(const Point2D<int> &size);
        void SetBackgroundColour(const glm::vec4 &colour);
        void SetForegroundColour(const glm::vec4 &colour);
        void SetFont(Graphics::Font *font);
        void SetFontSize(uint32_t fontSize);
        void SetMarginTop(uint32_t margin);
        void SetMarginBottom(uint32_t margin);
        void SetMarginLeft(uint32_t margin);
        void SetMarginRight(uint32_t margin);
        void SetMargin(uint32_t left, uint32_t right, uint32_t top, uint32_t bottom);

        const Point2D<int> &GetPosition() const;
        const Point2D<int> &GetSize() const;
        const glm::vec4 &GetBackgroundColour() const;
        const glm::vec4 &GetForegroundColour() const;
        const Graphics::Font *GetFont() const;
        const uint32_t GetFontSize() const;
        const uint32_t GetMarginTop() const;
        const uint32_t GetMarginBottom() const;
        const uint32_t GetMarginLeft() const;
        const uint32_t GetMarginRight() const;

        const Nexus::Graphics::Rectangle<float> GetBoundingRectangle() const;
        const Nexus::Graphics::Rectangle<float> GetBoundingRectangleTranslated() const;
        const Nexus::Graphics::Rectangle<float> GetScissor() const;
        const Nexus::Graphics::Rectangle<float> GetContentRegionAvailable() const;
        const Nexus::Graphics::Rectangle<float> GetContentRegionAvailableTranslated() const;

        virtual void SetCanvas(Canvas *canvas);
        const Canvas *const GetCanvas() const;

        void SetParent(Control *control);
        const Control *const GetParent() const;
        const glm::vec2 &GetChildOffset() const;

        // events
        EventHandler<Control *> OnClick;
        EventHandler<Control *> OnMouseEnter;
        EventHandler<Control *> OnMouseLeave;
        EventHandler<Control *> OnClose;
        EventHandler<Control *, Canvas *> OnAddedToCanvas;

    protected:
        Point2D<int> m_Position = {0, 0};
        Point2D<int> m_Size = {0, 0};
        uint32_t m_FontSize = 24;
        glm::vec4 m_BackgroundColour = {1, 1, 1, 1};
        glm::vec4 m_ForegroundColour = {0, 0, 0, 1};
        Graphics::Font *m_Font = nullptr;

        uint32_t m_MarginLeft = 0;
        uint32_t m_MarginTop = 0;
        uint32_t m_MarginRight = 0;
        uint32_t m_MarginBottom = 0;

        glm::vec2 m_ChildOffset = {0, 0};

        bool m_Hovered = false;
        bool m_Pressed = false;
        Control *m_Parent = nullptr;
        Canvas *m_Canvas = nullptr;

    private:
        friend class Canvas;
    };
}