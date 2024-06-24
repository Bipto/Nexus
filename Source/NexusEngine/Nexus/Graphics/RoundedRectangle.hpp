#pragma once

#include "Nexus/Point.hpp"

#include "Polygon.hpp"

#include <glm/glm.hpp>

namespace Nexus::Graphics
{
    struct RoundedRectangle
    {
    public:
        RoundedRectangle() = default;

        RoundedRectangle(const Point2D<float> &position, const Point2D<float> &size)
            : m_X(position.X), m_Y(position.Y),
              m_Width(size.X), m_Height(size.Y)
        {
        }

        RoundedRectangle(float x, float y, float width, float height)
            : m_X(x), m_Y(y),
              m_Width(width), m_Height(height)
        {
        }

        RoundedRectangle(const Point2D<float> &position, const Point2D<float> &size, float radiusTL, float radiusTR, float radiusBL, float radiusBR)
            : m_X(position.X), m_Y(position.Y),
              m_Width(size.X), m_Height(size.Y),
              m_RadiusTL(radiusTL), m_RadiusTR(radiusTR), m_RadiusBL(radiusBL), m_RadiusBR(radiusBR)
        {
        }

        RoundedRectangle(float x, float y, float width, float height, float radiusTL, float radiusTR, float radiusBL, float radiusBR)
            : m_X(x), m_Y(y),
              m_Width(width), m_Height(height),
              m_RadiusTL(radiusTL), m_RadiusTR(radiusTR), m_RadiusBL(radiusBL), m_RadiusBR(radiusBR)
        {
        }

        RoundedRectangle(const Rectangle<float> &rect, float radiusTL, float radiusTR, float radiusBL, float radiusBR)
            : m_X(rect.GetLeft()), m_Y(rect.GetTop()),
              m_Width(rect.GetWidth()), m_Height(rect.GetHeight()),
              m_RadiusTL(radiusTL), m_RadiusTR(radiusTR), m_RadiusBL(radiusBL), m_RadiusBR(radiusBR)
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

        const void Deconstruct(float *x, float *y, float *width, float *height, float *radiusTL, float *radiusTR, float *radiusBL, float *radiusBR, float *pointsPerCorner)
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

        std::vector<glm::vec2> CreateBorder() const
        {
            const glm::vec2 tr = {
                GetRight() - GetRadiusTopRight(),
                GetTop() + GetRadiusTopRight()};

            const glm::vec2 tl = {
                GetLeft() + GetRadiusTopLeft(),
                GetTop() + GetRadiusTopLeft()};

            const glm::vec2 bl = {
                GetLeft() + GetRadiusBottomLeft(),
                GetBottom() - GetRadiusBottomLeft()};

            const glm::vec2 br = {
                GetRight() - GetRadiusBottomRight(),
                GetBottom() - GetRadiusBottomRight()};

            std::vector<glm::vec2> border;

            if (GetRadiusTopLeft() > 0.0f)
            {
                std::vector<glm::vec2> triangleTL = CreateCircleRegion(tl, GetRadiusTopLeft(), 90.0f, 90.0f);
                border.insert(border.end(), triangleTL.begin(), triangleTL.end());
            }

            if (GetRadiusTopRight() > 0.0f)
            {
                std::vector<glm::vec2> triangleTR = CreateCircleRegion(tr, GetRadiusTopRight(), 0.0f, 90.0f);
                border.insert(border.end(), triangleTR.begin(), triangleTR.end());
            }

            if (GetRadiusBottomRight() > 0.0f)
            {
                std::vector<glm::vec2> triangleBR = CreateCircleRegion(br, GetRadiusBottomRight(), 270.0f, 90.0f);
                border.insert(border.end(), triangleBR.begin(), triangleBR.end());
            }

            if (GetRadiusBottomLeft() > 0.0f)
            {
                std::vector<glm::vec2> triangleBL = CreateCircleRegion(bl, GetRadiusBottomLeft(), 180.0f, 90.0f);
                border.insert(border.end(), triangleBL.begin(), triangleBL.end());
            }

            return border;
        }

        Polygon CreatePolygon() const
        {
            const glm::vec2 tr = {
                GetRight() - GetRadiusTopRight(),
                GetTop() + GetRadiusTopRight()};

            const glm::vec2 tl = {
                GetLeft() + GetRadiusTopLeft(),
                GetTop() + GetRadiusTopLeft()};

            const glm::vec2 bl = {
                GetLeft() + GetRadiusBottomLeft(),
                GetBottom() - GetRadiusBottomLeft()};

            const glm::vec2 br = {
                GetRight() - GetRadiusBottomRight(),
                GetBottom() - GetRadiusBottomRight()};

            const glm::vec2 centre =
                {
                    GetRight() - (GetWidth() / 2),
                    GetBottom() - (GetHeight() / 2)};

            std::vector<glm::vec2> points = CreateBorder();

            std::vector<Triangle2D> triangles;

            // create corners
            for (size_t i = 0; i < points.size(); i += 2)
            {
                const glm::vec2 &p0 = points[i];
                const glm::vec2 &p1 = points[(i + 1) % points.size()];

                Triangle2D t;
                t.A = p0;
                t.B = p1;
                t.C = centre;

                triangles.push_back(t);
            }

            // fill in gaps
            {
                Triangle2D top;
                top.A = {tl.x, tl.y - GetRadiusTopLeft()};
                top.B = {tr.x, tr.y - GetRadiusTopRight()};
                top.C = {centre.x, centre.y};
                triangles.push_back(top);

                Triangle2D left;
                left.A = {bl.x - GetRadiusBottomLeft(), bl.y};
                left.B = {tl.x - GetRadiusTopLeft(), tl.y};
                left.C = {centre.x, centre.y};
                triangles.push_back(left);

                Triangle2D right;
                right.A = {tr.x + GetRadiusTopRight(), tr.y};
                right.B = {br.x + GetRadiusBottomRight(), br.y};
                right.C = {centre.x, centre.y};
                triangles.push_back(right);

                Triangle2D bottom;
                bottom.A = {br.x, br.y + GetRadiusBottomRight()};
                bottom.B = {bl.x, bl.y + GetRadiusBottomLeft()};
                bottom.C = {centre.x, centre.y};
                triangles.push_back(bottom);
            }

            Polygon poly(triangles);
            return poly;
        }

    private:
        std::vector<glm::vec2> CreateCircleRegion(const glm::vec2 &position, float radius, float startAngle, float fillAngle) const
        {
            std::vector<glm::vec2> border;

            float deltaAngle = glm::radians(fillAngle) / (float)m_PointsPerCorner;
            float currentAngle = glm::radians(startAngle) + glm::radians(180.0f);

            const glm::vec2 topLeft = {position.x - (radius / 2), position.y - (radius / 2)};
            const glm::vec2 bottomRight = {position.x + (radius / 2), position.y + (radius / 2)};

            const glm::vec2 centre = {position.x, position.y};

            for (size_t i = 0; i < m_PointsPerCorner; i++)
            {
                glm::vec2 posA =
                    {
                        glm::sin(currentAngle) * radius + (position.x),
                        glm::cos(currentAngle) * radius + (position.y)};

                currentAngle -= deltaAngle;

                glm::vec2 posB =
                    {
                        glm::sin(currentAngle) * radius + (position.x),
                        glm::cos(currentAngle) * radius + (position.y)};

                border.push_back(posA);
                border.push_back(posB);
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
}