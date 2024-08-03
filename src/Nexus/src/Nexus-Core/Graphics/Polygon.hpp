#pragma once

#include "Triangle.hpp"
#include "Rectangle.hpp"

#include "Nexus-Core/Point.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
    struct Polygon
    {
    public:
        Polygon() = default;

        explicit Polygon(const std::vector<Triangle2D> &triangles)
            : m_Triangles(triangles)
        {
        }

        void SetTriangles(const std::vector<Triangle2D> &triangles)
        {
            m_Triangles = triangles;
        }

        const std::vector<Triangle2D> &GetTriangles() const
        {
            return m_Triangles;
        }

        bool Contains(const Nexus::Point2D<float> &point) const
        {
            for (const auto &tri : m_Triangles)
            {
                if (tri.Contains({point.X, point.Y}))
                {
                    return true;
                }
            }

            return false;
        }

        std::vector<glm::vec2> GetOutline() const
        {
            std::unordered_map<glm::vec2, uint32_t> vertexCounts;

            for (const Triangle2D &triangle : m_Triangles)
            {
                vertexCounts[triangle.A]++;
                vertexCounts[triangle.B]++;
                vertexCounts[triangle.C]++;
            }

            std::vector<glm::vec2> outline;

            for (const auto &[position, count] : vertexCounts)
            {
                if (count == 2)
                {
                    outline.push_back(position);
                }
            }

            return outline;
        }

        Rectangle<float> GetBoundingRectangle() const
        {
            glm::vec2 min = {FLT_MAX, FLT_MAX};
            glm::vec2 max = {-FLT_MAX, -FLT_MAX};

            for (const Triangle3D &tri : m_Triangles)
            {
                float maxX = glm::max<float>(tri.A.x, glm::max<float>(tri.B.x, tri.C.x));
                float minX = glm::min<float>(tri.A.x, glm::min<float>(tri.B.x, tri.C.x));

                float maxY = glm::max<float>(tri.A.y, glm::max<float>(tri.B.y, tri.C.y));
                float minY = glm::min<float>(tri.A.y, glm::min<float>(tri.B.y, tri.C.y));

                if (minX < min.x)
                {
                    min.x = minX;
                }

                if (minY < min.y)
                {
                    min.y = minY;
                }

                if (maxX > max.x)
                {
                    max.x = maxX;
                }

                if (maxY > max.y)
                {
                    max.y = maxY;
                }
            }

            Rectangle<float> rect(min.x, min.y, max.x - min.x, max.y - min.y);
            return rect;
        }

        Polygon ReverseWindingOrder() const
        {
            std::vector<Triangle2D> triangles(m_Triangles);
            std::reverse(triangles.begin(), triangles.end());
            return Polygon(triangles);
        }

    private:
        std::vector<Triangle2D> m_Triangles;
    };
}