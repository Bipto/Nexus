#include "Rectangle.hpp"
namespace Nexus::Graphics
{
    Rectangle::Rectangle(const Point<int> &position, const Point<int> &size)
        : m_X(position.X), m_Y(position.Y), m_Width(size.X), m_Height(size.Y)
    {
    }

    Rectangle::Rectangle(int x, int y, int width, int height)
        : m_X(x), m_Y(y), m_Width(width), m_Height(height)
    {
    }

    void Rectangle::SetX(int x)
    {
        m_X = x;
    }

    void Rectangle::SetY(int y)
    {
        m_Y = y;
    }

    void Rectangle::SetWidth(int width)
    {
        m_Width = width;
    }

    void Rectangle::SetHeight(int height)
    {
        m_Height = height;
    }

    const int Rectangle::GetLeft() const
    {
        return m_X;
    }

    const int Rectangle::GetRight() const
    {
        return m_X + m_Width;
    }

    const int Rectangle::GetTop() const
    {
        return m_Y;
    }

    const int Rectangle::GetBottom() const
    {
        return m_Y + m_Height;
    }

    const int Rectangle::GetHeight() const
    {
        return m_Height;
    }

    bool Rectangle::ContainsPoint(Nexus::Point<int> point)
    {
        return point.X >= m_X && point.X < m_X + m_Width &&
               point.Y >= m_Y && point.Y < m_Y + m_Height;
    }

    const int Rectangle::GetWidth() const
    {
        return m_Width;
    }
}