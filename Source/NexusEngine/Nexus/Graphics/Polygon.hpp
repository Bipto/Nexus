#pragma once

#include "Triangle.hpp"
#include "Rectangle.hpp"

#include "glm/glm.hpp"

#include <vector>

namespace Nexus::Graphics
{
    template <typename T>
    struct Polygon
    {
    public:
        Polygon() = default;

        explicit Polygon(const std::vector<Triangle> &triangles)
            : m_Triangles(triangles)
        {
        }

        void SetTriangles(const std::vector<Triangle> &triangles)
        {
            m_Triangles(triangles);
        }

        const std::vector<Triangle> &GetTriangles() const
        {
            return m_Triangles;
        }

        Rectangle<T> GetBoundingRectangle() const
        {
            glm::vec2 min = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
            glm::vec2 max = {-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()};

            for (const Triangle &tri : m_Triangles)
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

            Rectangle<T> rect(min.x, min.y, max.x - min.x, max.y - min.y);
            return rect;
        }

    private:
        std::vector<Triangle> m_Triangles;
    };
}