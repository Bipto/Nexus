#include "Control.hpp"

namespace Nexus::UI
{
    void Control::SetPosition(const Point2D<float> position)
    {
        m_Position = position;
    }

    void Control::SetSize(const Point2D<float> size)
    {
        m_Size = size;
    }

    void Nexus::UI::Control::SetFont(Graphics::Font *font)
    {
        m_Font = font;
    }

    void Control::SetFontSize(uint32_t size)
    {
        m_FontSize = size;
    }

    void Control::SetBackgroundColour(const glm::vec4 &colour)
    {
        m_BackgroundColour = colour;
    }

    void Control::SetForegroundColour(const glm::vec4 &colour)
    {
        m_ForegroundColour = colour;
    }

    void Control::SetCanvas(Canvas *canvas)
    {
        m_Canvas = canvas;
    }

    void Control::SetAutoSize(bool enabled)
    {
        m_AutoSize = enabled;
    }

    void Control::SetCornerRounding(const float rounding)
    {
        m_CornerRounding = rounding;
    }

    void Control::SetBorderThickness(const float thickness)
    {
        m_BorderThickness = thickness;
    }

    void Control::SetBorderColour(const glm::vec4 &colour)
    {
        m_BorderColour = colour;
    }

    void Control::SetPadding(const Padding &padding)
    {
        m_Padding = padding;
    }

    const Point2D<float> &Control::GetPosition() const
    {
        return m_Position;
    }

    const Point2D<float> &Control::GetSize() const
    {
        return m_Size;
    }

    const Graphics::Font *const Nexus::UI::Control::GetFont() const
    {
        return m_Font;
    }

    const uint32_t Control::GetFontSize() const
    {
        return m_FontSize;
    }
    const glm::vec4 &Control::GetBackgroundColour() const
    {
        return m_BackgroundColour;
    }

    const glm::vec4 &Control::GetForegroundColour() const
    {
        return m_ForegroundColour;
    }

    const Canvas *const Control::GetCanvas() const
    {
        return m_Canvas;
    }

    const bool Control::IsAutoSized() const
    {
        return m_AutoSize;
    }

    const float Control::GetCornerRounding() const
    {
        return m_CornerRounding;
    }

    float Control::GetBorderThickness() const
    {
        return m_BorderThickness;
    }

    const glm::vec4 &Control::GetBorderColour() const
    {
        return m_BorderColour;
    }

    const Padding &Control::GetPadding() const
    {
        return m_Padding;
    }

    const Graphics::Rectangle<float> Control::GetControlBounds() const
    {
        return Graphics::Rectangle<float>(
            m_Position.X,
            m_Position.Y,
            m_Size.X + m_CornerRounding + m_Padding.Left + m_Padding.Right,
            m_Size.Y + m_CornerRounding + m_Padding.Top + m_Padding.Bottom);
    }

    const Graphics::Rectangle<float> Control::GetDrawableBounds() const
    {
        return Graphics::Rectangle<float>(
            m_Position.X + (m_CornerRounding / 2) + m_Padding.Left,
            m_Position.Y + (m_CornerRounding / 2) + m_Padding.Top,
            m_Size.X + m_CornerRounding + m_Padding.Right,
            m_Size.Y + m_CornerRounding + m_Padding.Bottom);
    }
}