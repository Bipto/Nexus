#pragma once

#include "Nexus/Point.hpp"

namespace Nexus::Graphics
{
    /// @brief A struct representing a rectangle with a position, width and height
    template <typename T>
    struct Rectangle
    {
    public:
        Rectangle() = default;

        Rectangle(const Point<T> &position, const Point<T> &size)
            : m_X(position.X), m_Y(position.Y), m_Width(size.X), m_Height(size.Y)
        {
        }

        Rectangle(T x, T y, T width, T height)
            : m_X(x), m_Y(y), m_Width(width), m_Height(height)
        {
        }

        void SetX(T x)
        {
            m_X = x;
        }
        void SetY(T y)
        {
            m_Y = y;
        }

        void SetWidth(T width)
        {
            m_Width = width;
        }

        void SetHeight(T height)
        {
            m_Height = height;
        }

        const T GetLeft() const
        {
            return m_X;
        }

        const T GetRight() const
        {
            return m_X + m_Width;
        }

        const T GetTop() const
        {
            return m_Y;
        }

        const T GetBottom() const
        {
            return m_Y + m_Height;
        }

        const T GetHeight() const
        {
            return m_Height;
        }

        bool ContainsPoint(Nexus::Point<T> point)
        {
            return point.X >= m_X && point.X < m_X + m_Width &&
                   point.Y >= m_Y && point.Y < m_Y + m_Height;
        }

        const T GetWidth() const
        {
            return m_Width;
        }

    private:
        T m_X = 0;
        T m_Y = 0;
        T m_Width = 0;
        T m_Height = 0;
    };
}