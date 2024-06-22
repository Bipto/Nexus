#pragma once

#include "Triangle.hpp"
#include "Rectangle.hpp"

#include "glm/glm.hpp"

#include <vector>

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

        Rectangle<float> GetBoundingRectangle() const
        {
            // glm::vec2 min = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
            // glm::vec2 max = {-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()};

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

    private:
        std::vector<Triangle2D> m_Triangles;
    };
}