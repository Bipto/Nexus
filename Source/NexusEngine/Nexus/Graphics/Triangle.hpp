#pragma once

#include "glm/glm.hpp"

namespace Nexus::Graphics
{
    struct Triangle2D
    {
    public:
        glm::vec2 A;
        glm::vec2 B;
        glm::vec2 C;
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