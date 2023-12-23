#pragma once

#include "Nexus/Point.hpp"

namespace Nexus::Graphics
{
    /// @brief A struct representing a rectangle with a position, width and height
    struct Rectangle
    {
    public:
        Rectangle() = default;
        Rectangle(int x, int y, int width, int height);

        void SetX(int x);
        void SetY(int y);
        void SetWidth(int width);
        void SetHeight(int height);

        const int GetLeft() const;
        const int GetRight() const;
        const int GetTop() const;
        const int GetBottom() const;
        const int GetWidth() const;
        const int GetHeight() const;

        bool ContainsPoint(Nexus::Point<int> point);

    private:
        int m_X = 0;
        int m_Y = 0;
        int m_Width = 0;
        int m_Height = 0;
    };
}