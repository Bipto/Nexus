#include "Control.hpp"

namespace Nexus::UI
{
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
    const Graphics::Font *Control::GetFont() const
    {
        return m_Font;
    }
}
