#pragma once

#include "Nexus/Point.hpp"
#include "glm/glm.hpp"

namespace Nexus::Graphics
{
    struct Triangle2D
    {
    public:
        glm::vec2 A;
        glm::vec2 B;
        glm::vec2 C;

        bool Contains(const glm::vec2 &point) const
        {
            float a0 = EdgeFunction(B, C, point);
            float a1 = EdgeFunction(C, A, point);
            float a2 = EdgeFunction(A, B, point);

            bool overlap = true;
            overlap &= a0 > 0.0f;
            overlap &= a1 > 0.0f;
            overlap &= a2 > 0.0f;

            return overlap;
        }

    private:
        float EdgeFunction(const glm::vec2 &a, const glm::vec2 &c, const glm::vec2 &b) const
        {
            return ((c.x - a.x) * (b.y - a.y)) - ((c.y - a.y) * (b.x - a.x));
        }
    };

    struct Triangle3D
    {
    public:
        glm::vec3 A;
        glm::vec3 B;
        glm::vec3 C;

        Triangle3D(const Triangle2D &tri)
            : A(tri.A.x, tri.A.y, 0.0f),
              B(tri.B.x, tri.B.y, 0.0f),
              C(tri.C.x, tri.C.y, 0.0f)
        {
        }
    };
}