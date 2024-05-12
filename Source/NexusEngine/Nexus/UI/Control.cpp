#include "Control.hpp"

#include "Nexus/Input/Input.hpp"

namespace Nexus::UI
{
    void Control::Update()
    {
        Graphics::Rectangle rect(m_Position.X, m_Position.Y, m_Size.X, m_Size.Y);

        if (rect.ContainsPoint(Input::GetMousePosition()))
        {
            if (!m_Hovered)
            {
                OnMouseEnter.Invoke(this);
            }

            m_Hovered = true;
        }
        else
        {
            if (m_Hovered)
            {
                OnMouseLeave.Invoke(this);
            }

            m_Hovered = false;
        }

        if (m_Hovered && Input::IsLeftMouseReleased())
        {
            OnClick.Invoke(this);
        }
    }

    void Control::Render(Graphics::BatchRenderer *batchRenderer)
    {
    }

    void Control::SetPosition(const Point<int> &position)
    {
        m_Position = position;
    }

    void Control::SetSize(const Point<int> &size)
    {
        m_Size = size;
    }

    void Control::SetBackgroundColour(const glm::vec4 &colour)
    {
        m_BackgroundColour = colour;
    }

    void Control::SetForegroundColour(const glm::vec4 &colour)
    {
        m_ForegroundColour = colour;
    }

    void Control::SetFont(Graphics::Font *font)
    {
        m_Font = font;
    }

    void Control::SetFontSize(uint32_t fontSize)
    {
        m_FontSize = fontSize;
    }

    void Control::SetMarginTop(uint32_t margin)
    {
        m_MarginTop = margin;
    }

    void Control::SetMarginBottom(uint32_t margin)
    {
        m_MarginBottom = margin;
    }

    void Control::SetMarginLeft(uint32_t margin)
    {
        m_MarginLeft = margin;
    }

    void Control::SetMarginRight(uint32_t margin)
    {
        m_MarginRight = margin;
    }

    void Control::SetMargin(uint32_t left, uint32_t right, uint32_t top, uint32_t bottom)
    {
        SetMarginTop(top);
        SetMarginBottom(bottom);
        SetMarginLeft(left);
        SetMarginRight(right);
    }

    const Point<int> &Control::GetPosition() const
    {
        return m_Position;
    }
    const Point<int> &Control::GetSize() const
    {
        return m_Size;
    }

    const glm::vec4 &Control::GetBackgroundColour() const
    {
        return m_BackgroundColour;
    }
    const glm::vec4 &Control::GetForegroundColour() const
    {
        return m_ForegroundColour;
    }
    const Graphics::Font *Control::GetFont() const
    {
        return m_Font;
    }

    const uint32_t Control::GetFontSize() const
    {
        return m_FontSize;
    }

    const uint32_t Control::GetMarginTop() const
    {
        return m_MarginTop;
    }

    const uint32_t Control::GetMarginBottom() const
    {
        return m_MarginBottom;
    }

    const uint32_t Control::GetMarginLeft() const
    {
        return m_MarginLeft;
    }

    const uint32_t Control::GetMarginRight() const
    {
        return m_MarginRight;
    }

    const Nexus::Graphics::Rectangle<float> Control::GetRectangle() const
    {
        return Nexus::Graphics::Rectangle<float>(m_Position.X, m_Position.Y, m_Size.X, m_Size.Y);
    }

    const Canvas *Control::GetCanvas() const
    {
        return m_Canvas;
    }
}
