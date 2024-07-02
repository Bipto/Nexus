#pragma once

#include "Nexus/Graphics/SamplerState.hpp"
#include "glm/glm.hpp"

namespace Nexus::Utils
{
    glm::vec4 ColorFromRGBA(float r, float g, float b, float a);
    glm::vec4 ColorFromBorderColor(Nexus::Graphics::BorderColor color);

    template <typename T>
    inline T ReMapRange(T oldMin, T oldMax, T newMin, T newMax, T value)
    {
        return newMin + (newMax - newMin) * ((value - oldMin) / (oldMax - oldMin));
    }

    template <typename T>
    inline T Lerp(T x, T y, T t)
    {
        return x * (1 - t) + y * t;
    }

    inline float Dot(const glm::vec2 &a, const glm::vec2 &b)
    {
        return a.x * b.x + a.y * b.y;
    }

    inline float Cross(const glm::vec2 &a, const glm::vec2 &b)
    {
        return a.x * b.y - b.x * a.y;
    }

    inline bool IsInsideEdge(const glm::vec2 &point, const glm::vec2 &a, const glm::vec2 &b)
    {
        return (Cross(a - b, point) + Cross(b, a)) < 0.0f;
    }

    inline glm::vec2 Intersection(const glm::vec2 &a1, const glm::vec2 &a2, const glm::vec2 &b1, const glm::vec2 &b2)
    {
        return ((b1 - b2) * Cross(a1, a2) - (a1 - a2) * Cross(b1, b2)) * (1.0f / Cross(a1 - a2, b1 - b2));
    }
}