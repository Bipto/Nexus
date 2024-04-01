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
}
