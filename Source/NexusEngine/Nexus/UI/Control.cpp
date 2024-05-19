#include "Control.hpp"

#include "Nexus/Input/Input.hpp"

namespace Nexus::UI
{
    void Control::Update()
    {
        Graphics::Rectangle<float> rect = GetRectangle();
        const Point<int> mousePos = Input::GetMousePosition();

        if (rect.ContainsPoint({(float)mousePos.X, (float)mousePos.Y}))
        {
            if (!m_Hovered)
            {
                OnMouseEnter.Invoke(this);
            }

            m_Hovered = true;
            m_Pressed = Input::IsLeftMouseHeld();
        }
        else
        {
            if (m_Hovered)
            {
                OnMouseLeave.Invoke(this);
            }

            m_Hovered = false;
            m_Pressed = false;
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
        Nexus::Graphics::Rectangle<float> rect(m_Position.X, m_Position.Y, m_Size.X, m_Size.Y);

        if (m_Parent)
        {
            const auto &parentRectangle = m_Parent->GetRectangle();

            rect.SetX(parentRectangle.GetLeft() + m_Position.X);
            rect.SetY(parentRectangle.GetTop() + m_Position.Y);
        }

        return rect;
    }

    const Nexus::Graphics::Rectangle<float> Control::GetScissor() const
    {
        Nexus::Graphics::Rectangle<float> scissor = GetRectangle();

        if (m_Parent)
        {
            const auto &parentRectangle = m_Parent->GetScissor();

            if (parentRectangle.GetLeft() > scissor.GetLeft())
            {
                scissor.SetX(parentRectangle.GetLeft());
            }

            if (parentRectangle.GetTop() > scissor.GetTop())
            {
                scissor.SetY(parentRectangle.GetTop());
            }

            if (parentRectangle.GetRight() < scissor.GetRight())
            {
                float width = parentRectangle.GetRight() - scissor.GetLeft();
                scissor.SetWidth(width);
            }

            if (parentRectangle.GetBottom() < scissor.GetBottom())
            {
                float height = parentRectangle.GetBottom() - scissor.GetTop();
                scissor.SetHeight(height);
            }
        }

        return scissor;
    }

    const Canvas *const Control::GetCanvas() const
    {
        if (m_Parent)
        {
            return m_Parent->GetCanvas();
        }

        return m_Canvas;
    }

    void Control::SetParent(Control *control)
    {
        m_Parent = control;
        m_Canvas = m_Parent->m_Canvas;
    }

    const Control *const Control::GetParent() const
    {
        return m_Parent;
    }
}
