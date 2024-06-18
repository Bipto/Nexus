#pragma once

#include "Nexus/Point.hpp"

#include <glm/glm.hpp>

namespace Nexus::Graphics
{
    template <typename T>
    struct RoundedRectangle
    {
    public:
        RoundedRectangle() = default;

        RoundedRectangle(const Point<T> &position, const Point<T> &size)
            : m_X(position.X), m_Y(position.Y),
              m_Width(size.X), m_Height(size.Y)
        {
        }

        RoundedRectangle(const Point<T> &position, const Point<T> &size, T radiusTL, T radiusTR, T radiusBL, T radiusBR)
            : m_X(position.X), m_Y(position.Y),
              m_Width(size.X), m_Height(size.Y),
              m_RadiusTL(radiusTL), m_RadiusTR(radiusTR), m_RadiusBL(radiusBL), m_RadiusBR(radiusBR)
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

        void SetRadiusTopLeft(T radius)
        {
            m_RadiusTL = radius;
        }

        void SetRadiusTopRight(T radius)
        {
            m_RadiusTR = radius;
        }

        void SetRadiusBottomLeft(T radius)
        {
            m_RadiusBottomLeft = radius;
        }

        void SetRadiusBottomRight(T radius)
        {
            m_RadiusBottomRight = radius;
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

        const T GetWidth() const
        {
            return m_Width;
        }

        const T GetRadiusTopLeft() const
        {
            return m_RadiusTL;
        }

        const T GetRadiusTopRight() const
        {
            return m_RadiusTR;
        }

        const T GetRadiusBottomLeft() const
        {
            return m_RadiusBL;
        }

        const T GetRadiusBottomRight() const
        {
            return m_RadiusBR;
        }

    private:
        T m_X = 0;
        T m_Y = 0;
        T m_Width = 0;
        T m_Height = 0;
        T m_RadiusTL = 0.0f;
        T m_RadiusTR = 0.0f;
        T m_RadiusBL = 0.0f;
        T m_RadiusBR = 0.0f;
    };
}