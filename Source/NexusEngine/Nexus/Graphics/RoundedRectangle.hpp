#pragma once

#include "Nexus/Point.hpp"

#include "Polygon.hpp"

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
            m_RadiusBL = radius;
        }

        void SetRadiusBottomRight(T radius)
        {
            m_RadiusBR = radius;
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

        Polygon<float> CreatePolygon() const
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

            std::vector<Triangle> triangles;

            if (GetRadiusTopLeft() > 0.0f)
            {
                std::vector<Triangle> triangleTL = CreateCircleRegion(tl, GetRadiusTopLeft(), 90.0f, 90.0f);
                triangles.insert(triangles.end(), triangleTL.begin(), triangleTL.end());

                glm::vec3 connectionA = {tl.x, tl.y - GetRadiusTopLeft(), 0.0f};
                glm::vec3 connectionB = {tl.x, tl.y, 0.0f};
                glm::vec3 connectionC = {tl.x - GetRadiusTopLeft(), tl.y, 0.0f};

                Triangle circleToCentreA;
                circleToCentreA.A = connectionA;
                circleToCentreA.B = connectionB;
                circleToCentreA.C = {centre.x, centre.y, 0.0f};
                triangles.push_back(circleToCentreA);

                Triangle circleToCentreB;
                circleToCentreB.A = connectionB;
                circleToCentreB.B = connectionC;
                circleToCentreB.C = {centre.x, centre.y, 0.0f};
                triangles.push_back(circleToCentreB);
            }

            if (GetRadiusTopRight() > 0.0f)
            {
                std::vector<Triangle> triangleTR = CreateCircleRegion(tr, GetRadiusTopRight(), 0.0f, 90.0f);
                triangles.insert(triangles.end(), triangleTR.begin(), triangleTR.end());

                glm::vec3 connectionA = {tr.x, tr.y - GetRadiusTopRight(), 0.0f};
                glm::vec3 connectionB = {tr.x, tr.y, 0.0f};
                glm::vec3 connectionC = {tr.x + GetRadiusTopRight(), tr.y, 0.0f};

                Triangle circleToCentreA;
                circleToCentreA.A = connectionA;
                circleToCentreA.B = connectionB;
                circleToCentreA.C = {centre.x, centre.y, 0.0f};
                triangles.push_back(circleToCentreA);

                Triangle circleToCentreB;
                circleToCentreB.A = connectionB;
                circleToCentreB.B = connectionC;
                circleToCentreB.C = {centre.x, centre.y, 0.0f};
                triangles.push_back(circleToCentreB);
            }

            if (GetRadiusBottomLeft() > 0.0f)
            {
                std::vector<Triangle> triangleBL = CreateCircleRegion(bl, GetRadiusBottomLeft(), 180.0f, 90.0f);
                triangles.insert(triangles.end(), triangleBL.begin(), triangleBL.end());

                Triangle circleToCentreA;
                circleToCentreA.A = {bl.x - GetRadiusBottomLeft(), bl.y, 0.0f};
                circleToCentreA.B = {bl.x, bl.y, 0.0f};
                circleToCentreA.C = {centre.x, centre.y, 0.0f};
                triangles.push_back(circleToCentreA);

                Triangle circleToCentreB;
                circleToCentreB.A = {bl.x, bl.y, 0.0f};
                circleToCentreB.B = {bl.x, bl.y + GetRadiusBottomLeft(), 0.0f};
                circleToCentreB.C = {centre.x, centre.y, 0.0f};
                triangles.push_back(circleToCentreB);
            }

            if (GetRadiusBottomRight() > 0.0f)
            {
                std::vector<Triangle> triangleBR = CreateCircleRegion(br, GetRadiusBottomRight(), 270.0f, 90.0f);
                triangles.insert(triangles.end(), triangleBR.begin(), triangleBR.end());

                Triangle circleToCentreA;
                circleToCentreA.A = {br.x, br.y + GetRadiusBottomRight(), 0.0f};
                circleToCentreA.B = {br.x, br.y, 0.0f};
                circleToCentreA.C = {centre.x, centre.y, 0.0f};
                triangles.push_back(circleToCentreA);

                Triangle circleToCentreB;
                circleToCentreB.A = {br.x, br.y, 0.0f};
                circleToCentreB.B = {br.x + GetRadiusBottomRight(), br.y, 0.0f};
                circleToCentreB.C = {centre.x, centre.y, 0.0f};
                triangles.push_back(circleToCentreB);
            }

            // fill in missing gaps
            {
                Triangle top;
                top.A = {tl.x, tl.y - GetRadiusTopLeft(), 0.0f};
                top.B = {tr.x, tr.y - GetRadiusTopRight(), 0.0f};
                top.C = {centre.x, centre.y, 0.0f};
                triangles.push_back(top);

                Triangle left;
                left.A = {tl.x - GetRadiusTopLeft(), tl.y, 0.0f};
                left.B = {bl.x - GetRadiusBottomLeft(), bl.y, 0.0f};
                left.C = {centre.x, centre.y, 0.0f};
                triangles.push_back(left);

                Triangle right;
                right.A = {tr.x + GetRadiusTopRight(), tr.y, 0.0f};
                right.B = {br.x + GetRadiusBottomRight(), br.y, 0.0f};
                right.C = {centre.x, centre.y, 0.0f};
                triangles.push_back(right);

                Triangle bottom;
                bottom.A = {bl.x, bl.y + GetRadiusBottomLeft(), 0.0f};
                bottom.B = {br.x, br.y + GetRadiusBottomRight(), 0.0f};
                bottom.C = {centre.x, centre.y, 0.0f};
                triangles.push_back(bottom);
            }

            Polygon<float> poly(triangles);
            return poly;
        }

    private:
        std::vector<Triangle> CreateCircleRegion(const glm::vec2 &position, float radius, float startAngle, float fillAngle) const
        {
            std::vector<Triangle> triangles;

            float deltaAngle = glm::radians(fillAngle) / (float)m_PointsPerCorner;
            float currentAngle = glm::radians(startAngle) + glm::radians(180.0f);

            const glm::vec2 topLeft = {position.x - (radius / 2), position.y - (radius / 2)};
            const glm::vec2 bottomRight = {position.x + (radius / 2), position.y + (radius / 2)};

            const glm::vec3 centre = {position.x, position.y, 0.0f};

            for (size_t i = 0; i < m_PointsPerCorner; i++)
            {
                glm::vec3 posA =
                    {
                        glm::sin(currentAngle) * radius + (position.x),
                        glm::cos(currentAngle) * radius + (position.y),
                        0.0f};

                currentAngle -= deltaAngle;

                glm::vec3 posB =
                    {
                        glm::sin(currentAngle) * radius + (position.x),
                        glm::cos(currentAngle) * radius + (position.y),
                        0.0f};

                Triangle tri;
                tri.A = centre;
                tri.B = posA;
                tri.C = posB;
                triangles.push_back(tri);
            }

            return triangles;
        }

    private:
        T m_X = 0;
        T m_Y = 0;
        T m_Width = 0;
        T m_Height = 0;
        T m_RadiusTL = 0;
        T m_RadiusTR = 0;
        T m_RadiusBL = 0;
        T m_RadiusBR = 0;
        T m_PointsPerCorner = 16;
    };
}