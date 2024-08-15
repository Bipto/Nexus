#pragma once

#include "Nexus-Core/Point.hpp"
#include "Nexus-Core/Utils/Utils.hpp"

#include "Polygon.hpp"
#include "Rectangle.hpp"

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
struct RoundedRectangle
{
  public:
    RoundedRectangle() = default;

    RoundedRectangle(const Point2D<float> &position, const Point2D<float> &size) : m_X(position.X), m_Y(position.Y), m_Width(size.X), m_Height(size.Y)
    {
    }

    RoundedRectangle(float x, float y, float width, float height) : m_X(x), m_Y(y), m_Width(width), m_Height(height)
    {
    }

    RoundedRectangle(const Point2D<float> &position, const Point2D<float> &size, float radiusTL, float radiusTR, float radiusBL, float radiusBR)
        : m_X(position.X), m_Y(position.Y), m_Width(size.X), m_Height(size.Y), m_RadiusTL(radiusTL), m_RadiusTR(radiusTR), m_RadiusBL(radiusBL), m_RadiusBR(radiusBR)
    {
    }

    RoundedRectangle(float x, float y, float width, float height, float radiusTL, float radiusTR, float radiusBL, float radiusBR)
        : m_X(x), m_Y(y), m_Width(width), m_Height(height), m_RadiusTL(radiusTL), m_RadiusTR(radiusTR), m_RadiusBL(radiusBL), m_RadiusBR(radiusBR)
    {
    }

    RoundedRectangle(const Rectangle<float> &rect, float radiusTL, float radiusTR, float radiusBL, float radiusBR)
        : m_X(rect.GetLeft()), m_Y(rect.GetTop()), m_Width(rect.GetWidth()), m_Height(rect.GetHeight()), m_RadiusTL(radiusTL), m_RadiusTR(radiusTR), m_RadiusBL(radiusBL),
          m_RadiusBR(radiusBR)
    {
    }

    void SetPointsPerCorner(size_t points)
    {
        m_PointsPerCorner = points;
    }

    const size_t GetPointsPerCorner() const
    {
        return m_PointsPerCorner;
    }

    void SetX(float x)
    {
        m_X = x;
    }

    void SetY(float y)
    {
        m_Y = y;
    }

    void SetWidth(float width)
    {
        m_Width = width;
    }

    void SetHeight(float height)
    {
        m_Height = height;
    }

    void SetRadiusTopLeft(float radius)
    {
        m_RadiusTL = radius;
    }

    void SetRadiusTopRight(float radius)
    {
        m_RadiusTR = radius;
    }

    void SetRadiusBottomLeft(float radius)
    {
        m_RadiusBL = radius;
    }

    void SetRadiusBottomRight(float radius)
    {
        m_RadiusBR = radius;
    }

    const float GetLeft() const
    {
        return m_X;
    }

    const float GetRight() const
    {
        return m_X + m_Width;
    }

    const float GetTop() const
    {
        return m_Y;
    }

    const float GetBottom() const
    {
        return m_Y + m_Height;
    }

    const float GetHeight() const
    {
        return m_Height;
    }

    const float GetWidth() const
    {
        return m_Width;
    }

    const float GetRadiusTopLeft() const
    {
        return m_RadiusTL;
    }

    const float GetRadiusTopRight() const
    {
        return m_RadiusTR;
    }

    const float GetRadiusBottomLeft() const
    {
        return m_RadiusBL;
    }

    const float GetRadiusBottomRight() const
    {
        return m_RadiusBR;
    }

    glm::vec2 GetTopLeft() const
    {
        return {GetLeft(), GetTop()};
    }

    glm::vec2 GetTopRight() const
    {
        return {GetRight(), GetTop()};
    }

    glm::vec2 GetBottomLeft() const
    {
        return {GetLeft(), GetBottom()};
    }

    glm::vec2 GetBottomRight() const
    {
        return {GetRight(), GetBottom()};
    }

    const bool Contains(const Nexus::Point2D<float> &point) const
    {
        Nexus::Graphics::Polygon poly = CreatePolygon();
        return poly.Contains(point);
    }

    void Inflate(float horizontalAmount, float verticalAmount)
    {
        m_X -= horizontalAmount;
        m_Y -= verticalAmount;
        m_Width += horizontalAmount * 2;
        m_Height += verticalAmount * 2;
    }

    void Offset(float x, float y)
    {
        m_X += x;
        m_Y += y;
    }

    const void Deconstruct(float *x, float *y, float *width, float *height, float *radiusTL, float *radiusTR, float *radiusBL, float *radiusBR, float *pointsPerCorner) const
    {
        if (x)
        {
            *x = m_X;
        }

        if (y)
        {
            *y = m_Y;
        }

        if (width)
        {
            *width = m_Width;
        }

        if (height)
        {
            *height = m_Height;
        }

        if (radiusTL)
        {
            *radiusTL = m_RadiusTL;
        }

        if (radiusTR)
        {
            *radiusTR = m_RadiusTR;
        }

        if (radiusBL)
        {
            *radiusBL = m_RadiusBL;
        }

        if (radiusBR)
        {
            *radiusBR = m_RadiusBR;
        }

        if (pointsPerCorner)
        {
            *pointsPerCorner = m_PointsPerCorner;
        }
    }

    std::vector<glm::vec2> CreateOutline() const
    {
        const glm::vec2 tr = {GetRight() - GetRadiusTopRight(), GetTop() + GetRadiusTopRight()};

        const glm::vec2 tl = {GetLeft() + GetRadiusTopLeft(), GetTop() + GetRadiusTopLeft()};

        const glm::vec2 bl = {GetLeft() + GetRadiusBottomLeft(), GetBottom() - GetRadiusBottomLeft()};

        const glm::vec2 br = {GetRight() - GetRadiusBottomRight(), GetBottom() - GetRadiusBottomRight()};

        std::vector<glm::vec2> points;

        if (m_RadiusTL > 0.0f)
        {
            std::vector<glm::vec2> corner = CreateCircleRegion(tl, m_RadiusTL, 0.0f, 90.0f);
            points.insert(points.end(), corner.begin(), corner.end());
        }
        else
        {
            points.push_back({tl.x - m_RadiusTL, tl.y + m_RadiusTL});
        }

        if (m_RadiusTR > 0.0f)
        {
            std::vector<glm::vec2> corner = CreateCircleRegion(tr, m_RadiusTR, 90.0f, 90.0f);
            points.insert(points.end(), corner.begin(), corner.end());
        }
        else
        {
            points.push_back({tr.x + m_RadiusTR, tr.y + m_RadiusTR});
        }

        if (m_RadiusBR > 0.0f)
        {
            std::vector<glm::vec2> corner = CreateCircleRegion(br, m_RadiusBR, 180.0f, 90.0f);
            points.insert(points.end(), corner.begin(), corner.end());
        }
        else
        {
            points.push_back({br.x + m_RadiusBR, br.y + m_RadiusBR});
        }

        if (m_RadiusBL > 0.0f)
        {
            std::vector<glm::vec2> corner = CreateCircleRegion(bl, m_RadiusBL, 270.0f, 90.0f);
            points.insert(points.end(), corner.begin(), corner.end());
        }
        else
        {
            points.push_back({bl.x - m_RadiusBL, bl.y + m_RadiusBL});
        }

        return points;
    }

    Polygon CreatePolygon() const
    {
        const std::vector<glm::vec2> &points = CreateOutline();
        return Nexus::Utils::GeneratePolygon(points);
    }

    Rectangle<float> GetBoundingRectangle() const
    {
        return Rectangle<float>(m_X, m_Y, m_Width, m_Height);
    }

    std::vector<glm::vec2> ClipAgainst(RoundedRectangle clip) const
    {
        std::vector<glm::vec2> subjectPoints = CreateOutline();
        std::vector<glm::vec2> clipPoints = clip.CreateOutline();

        return Nexus::Utils::SutherlandHodgman(subjectPoints, clipPoints);
    }

  private:
    std::vector<glm::vec2> CreateCircleRegion(const glm::vec2 &position, float radius, float startAngle, float fillAngle) const
    {
        std::vector<glm::vec2> border;

        float deltaAngle = glm::radians(fillAngle) / (float)m_PointsPerCorner;
        float currentAngle = glm::radians(startAngle) + glm::radians(180.0f);

        const glm::vec2 centre = {position.x, position.y};

        for (uint32_t i = 0; i < m_PointsPerCorner; i++)
        {
            glm::vec2 point = {glm::cos(currentAngle) * radius + position.x, glm::sin(currentAngle) * radius + position.y};

            border.push_back(point);
            currentAngle += deltaAngle;
        }

        return border;
    }

  private:
    float m_X = 0;
    float m_Y = 0;
    float m_Width = 0;
    float m_Height = 0;
    float m_RadiusTL = 0;
    float m_RadiusTR = 0;
    float m_RadiusBL = 0;
    float m_RadiusBR = 0;
    size_t m_PointsPerCorner = 16;
};
} // namespace Nexus::Graphics