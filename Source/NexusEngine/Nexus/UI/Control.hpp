#pragma once

#include "Nexus/Renderer/BatchRenderer.hpp"

#include "Nexus/Graphics/Font.hpp"

#include "Nexus/Point.hpp"
#include "Nexus/Graphics/Rectangle.hpp"

#include "Canvas.hpp"

#include "glm/glm.hpp"

namespace Nexus::UI
{
    class Canvas;

    class Control
    {
    public:
        Control() = default;
        virtual ~Control() {}
        virtual void OnUpdate() = 0;
        virtual void OnRender(Nexus::Graphics::BatchRenderer *renderer) = 0;

        // setters
        void SetPosition(const Point2D<float> position);
        void SetSize(const Point2D<float> size);
        void SetFont(Graphics::Font *font);
        void SetFontSize(uint32_t size);
        void SetBackgroundColour(const glm::vec4 &colour);
        void SetForegroundColour(const glm::vec4 &colour);
        void SetCanvas(Canvas *canvas);
        void SetAutoSize(bool enabled);

        // getters
        const Point2D<float> &GetPosition() const;
        const Point2D<float> &GetSize() const;
        const Graphics::Font *const GetFont() const;
        const uint32_t GetFontSize() const;
        const glm::vec4 &GetBackgroundColour() const;
        const glm::vec4 &GetForegroundColour() const;
        const Canvas *const GetCanvas() const;
        const bool IsAutoSized() const;

        // utilities
        const Graphics::Rectangle<float> GetControlBounds() const;

    protected:
        Point2D<float> m_Position = {0, 0};
        Point2D<float> m_Size = {0, 0};
        uint32_t m_FontSize = 24;
        Graphics::Font *m_Font = nullptr;

        glm::vec4 m_BackgroundColour = {1.0f, 1.0f, 1.0f, 1.0f};
        glm::vec4 m_ForegroundColour = {0.0f, 0.0f, 0.0f, 1.0f};

        Canvas *m_Canvas = nullptr;
        bool m_AutoSize = false;
    };
}