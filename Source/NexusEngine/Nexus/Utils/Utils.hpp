#pragma once

#include "Nexus/Graphics/SamplerState.hpp"
#include "Nexus/Graphics/Triangle.hpp"
#include "Nexus/Graphics/Polygon.hpp"
#include "Nexus/Graphics/WindingOrder.hpp"

#include "Nexus/nxpch.hpp"

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

    void Clip(std::vector<glm::vec2> &points, float x1, float y1, float x2, float y2);

    std::vector<glm::vec2> SutherlandHodgman(const std::vector<glm::vec2> &subjectPolygon, const std::vector<glm::vec2> &clipPolygon);

    float FindPolygonArea(std::span<glm::vec2> polygon);

    bool Triangulate(const std::vector<glm::vec2> &polygon, std::vector<uint32_t> &triangles);

    std::vector<Nexus::Graphics::Triangle2D> GenerateGeometry(const std::vector<glm::vec2> &polygon, const std::vector<uint32_t> &indices);

    Nexus::Graphics::Polygon GeneratePolygon(const std::vector<glm::vec2> &polygon);

    Graphics::WindingOrder GetWindingOrder(glm::vec2 a, glm::vec2 b, glm::vec2 c);

    std::vector<glm::vec2> ReverseWindingOrder(const std::vector<glm::vec2> &vertices);

    template <typename T>
    T GetItem(std::span<T> collection, int index)
    {
        int32_t size = static_cast<int32_t>(collection.size());

        if (index >= size)
        {
            return collection[index % collection.size()];
        }
        else if (index < 0)
        {
            uint32_t i = collection.size() - (index * -1);
            return collection[i];
        }
        else
        {
            return collection[index];
        }
    }
}