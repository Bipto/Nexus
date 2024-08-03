#pragma once

#include "Nexus-Core/Renderer/BatchRenderer.hpp"
#include "Nexus-Core/Graphics/Font.hpp"
#include "Nexus-Core/Point.hpp"
#include "Nexus-Core/Graphics/Rectangle.hpp"
#include "Nexus-Core/Events/EventHandler.hpp"
#include "Nexus-Core/Input/Input.hpp"

#include "Canvas.hpp"

#include "glm/glm.hpp"

namespace Nexus::UI
{
    class Canvas;

    struct Padding
    {
        float Left = 0.0f;
        float Right = 0.0f;
        float Top = 0.0f;
        float Bottom = 0.0f;

        Padding() = default;

        explicit Padding(float padding)
            : Left(padding), Right(padding), Top(padding), Bottom(padding)
        {
        }

        explicit Padding(float left, float right, float top, float bottom)
            : Left(left), Right(right), Top(top), Bottom(bottom)
        {
        }

        explicit Padding(float vertical, float horizontal)
            : Left(vertical), Right(vertical), Top(horizontal), Bottom(horizontal)
        {
        }
    };

    struct MouseClick
    {
        Nexus::Mouse Button;
        Point2D<int> Position;
    };

    class Control
    {
    public:
        Control() = default;
        virtual ~Control() {}
        virtual void OnUpdate() = 0;
        virtual void OnRender(Nexus::Graphics::BatchRenderer *renderer) = 0;

        virtual void OnAutoSize() = 0;
        virtual void HandleMouseClick(const MouseClick &e) = 0;

        // setters
        void SetLocalPosition(const Point2D<float> position);
        void SetSize(const Point2D<float> size);
        void SetFont(Graphics::Font *font);
        void SetFontSize(uint32_t size);
        void SetBackgroundColour(const glm::vec4 &colour);
        void SetForegroundColour(const glm::vec4 &colour);
        void SetAutoSize(bool enabled);
        void SetCornerRounding(const float rounding);
        void SetBorderThickness(const float thickness);
        void SetBorderColour(const glm::vec4 &colour);
        void SetPadding(const Padding &padding);

        // getters
        const Point2D<float> GetLocalPosition() const;
        const Point2D<float> GetScreenSpacePosition() const;
        const Point2D<float> &GetSize() const;
        const Graphics::Font *const GetFont() const;
        const uint32_t GetFontSize() const;
        const glm::vec4 &GetBackgroundColour() const;
        const glm::vec4 &GetForegroundColour() const;
        const Canvas *const GetCanvas() const;
        const bool IsAutoSized() const;
        const float GetCornerRounding() const;
        float GetBorderThickness() const;
        const glm::vec4 &GetBorderColour() const;
        const Padding &GetPadding() const;
        Graphics::RoundedRectangle GetRoundedRectangle() const;

        // utilities
        const Graphics::Rectangle<float> GetControlBounds() const;
        const Graphics::Rectangle<float> GetContentBounds() const;
        const Graphics::Scissor GetScissorRectangle() const;
        const std::vector<glm::vec2> GetDrawableArea() const;

        void AddControl(Control *control);
        void RemoveControl(Control *control);
        const std::vector<Control *> GetControls() const;

        EventHandler<Control *> OnMouseEnter;
        EventHandler<Control *> OnMouseLeave;
        EventHandler<Control *> OnMouseClick;

    private:
        void SetCanvas(Canvas *canvas);

    protected:
        Point2D<float> m_LocalPosition = {0, 0};
        Point2D<float> m_Size = {0, 0};
        uint32_t m_FontSize = 24;
        Graphics::Font *m_Font = nullptr;

        glm::vec4 m_BackgroundColour = {1.0f, 1.0f, 1.0f, 1.0f};
        glm::vec4 m_ForegroundColour = {0.0f, 0.0f, 0.0f, 1.0f};
        glm::vec4 m_BorderColour = {0.0f, 0.0f, 0.0f, 1.0f};

        Canvas *m_Canvas = nullptr;
        bool m_AutoSize = false;
        bool m_ContainsMouse = false;
        float m_CornerRounding = 0.0f;
        float m_BorderThickness = 0.0f;

        Padding m_Padding;
        std::vector<Control *> m_Controls;
        Control *m_Parent = nullptr;

        friend class Canvas;
    };
}