#pragma once

#include "Nexus/Graphics/SamplerState.hpp"
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

    inline float XIntersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
    {
        float num = (x1 * y2 - y1 * x2) * (x3 - x4) -
                    (x1 - x2) * (x3 * y4 - y3 * x4);
        float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        return num / den;
    }

    inline float YIntersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
    {
        float num = (x1 * y2 - y1 * x2) * (y3 - y4) -
                    (y1 - y2) * (x3 * y4 - y3 * x4);
        float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        return num / den;
    }

    inline void Clip(std::vector<glm::vec2> &points, float x1, float y1, float x2, float y2)
    {
        std::vector<glm::vec2> newPoints;

        for (size_t i = 0; i < points.size(); i++)
        {
            float k = (i + 1) % points.size();
            glm::vec2 pi = points[i];
            glm::vec2 pk = points[k];

            float i_pos = (x2 - x1) * (pi.y - y1) - (y2 - y1) * (pi.x - x1);

            float k_pos = (x2 - x1) * (pk.y - y1) - (y2 - y1) * (pk.x - x1);

            if (i_pos < 0 && k_pos < 0)
            {
                newPoints.push_back(pk);
            }
            else if (i_pos >= 0 && k_pos < 0)
            {
                float newX = XIntersect(x1, y1, x2, y2, pi.x, pi.y, pk.x, pk.y);
                float newY = YIntersect(x1, y1, x2, y2, pi.x, pi.y, pk.x, pk.y);
                newPoints.push_back(glm::vec2(newX, newY));

                newPoints.push_back(pk);
            }
            else if (i_pos < 0 && k_pos >= 0)
            {
                float newX = XIntersect(x1, y1, x2, y2, pi.x, pi.y, pk.x, pk.y);
                float newY = YIntersect(x1, y1, x2, y2, pi.x, pi.y, pk.x, pk.y);
                newPoints.push_back(glm::vec2(newX, newY));
            }
        }

        points = newPoints;
    }

    inline std::vector<glm::vec2> SutherlandHodgman(const std::vector<glm::vec2> &subjectPolygon, const std::vector<glm::vec2> &clipPolygon)
    {
        std::vector<glm::vec2> poly = subjectPolygon;

        for (size_t i = 0; i < clipPolygon.size(); i++)
        {
            float k = (i + 1) % clipPolygon.size();

            Clip(poly, clipPolygon[i].x, clipPolygon[i].y, clipPolygon[k].x, clipPolygon[k].y);
        }

        return poly;
    }
}