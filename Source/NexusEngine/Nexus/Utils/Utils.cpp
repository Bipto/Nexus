#include "Utils.hpp"

namespace Nexus::Utils
{
    glm::vec4 ColorFromRGBA(float r, float g, float b, float a)
    {
        return glm::vec4(
            1.0f / 255.0f * r,
            1.0f / 255.0f * g,
            1.0f / 255.0f * b,
            1.0f / 255.0f * a);
    }

    glm::vec4 ColorFromBorderColor(Nexus::Graphics::BorderColor color)
    {
        switch (color)
        {
        case Nexus::Graphics::BorderColor::TransparentBlack:
            return {0.0f, 0.0f, 0.0f, 0.0f};
        case Nexus::Graphics::BorderColor::OpaqueBlack:
            return {0.0f, 0.0f, 0.0f, 1.0f};
        case Nexus::Graphics::BorderColor::OpaqueWhite:
            return {1.0f, 1.0f, 1.0f, 1.0f};
        default:
            throw std::runtime_error("Failed to find a valid border color");
        }
    }

    float XIntersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
    {
        float num = (x1 * y2 - y1 * x2) * (x3 - x4) -
                    (x1 - x2) * (x3 * y4 - y3 * x4);
        float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        return num / den;
    }

    float YIntersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
    {
        float num = (x1 * y2 - y1 * x2) * (y3 - y4) -
                    (y1 - y2) * (x3 * y4 - y3 * x4);
        float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        return num / den;
    }

    void Clip(std::vector<glm::vec2> &points, float x1, float y1, float x2, float y2)
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

    std::vector<glm::vec2> SutherlandHodgman(const std::vector<glm::vec2> &subjectPolygon, const std::vector<glm::vec2> &clipPolygon)
    {
        assert(subjectPolygon.size() >= 3 && "The subject polygon must have at least 3 points");
        assert(clipPolygon.size() >= 3 && "The clipping polygon must have at least 3 points");

        std::vector<glm::vec2> poly(subjectPolygon);
        std::vector<glm::vec2> clip(clipPolygon);

        WindingOrder subjectWindingOrder = GetWindingOrder(poly[0], poly[1], poly[2]);
        WindingOrder clipWindingOrder = GetWindingOrder(clip[0], clip[1], clip[2]);

        if (subjectWindingOrder == WindingOrder::CounterClockwise)
        {
            poly = ReverseWindingOrder(poly);
        }

        if (clipWindingOrder == WindingOrder::CounterClockwise)
        {
            clip = ReverseWindingOrder(clipPolygon);
        }

        for (size_t i = 0; i < clip.size(); i++)
        {
            float k = (i + 1) % clip.size();

            Clip(poly, clip[i].x, clip[i].y, clip[k].x, clip[k].y);
        }

        return poly;
    }

    float FindPolygonArea(std::span<glm::vec2> polygon)
    {
        float totalArea = 0.0f;

        for (size_t i = 0; i < polygon.size(); i++)
        {
            glm::vec2 a = polygon[i];
            glm::vec2 b = polygon[(i + 1) % polygon.size()];

            float dy = (a.y + b.y) / 2.0f;
            float dx = (b.x - a.x);

            float area = dy * dx;
            totalArea += area;
        }

        return glm::abs(totalArea);
    }

    bool IsPointInTriangle(glm::vec2 p, glm::vec2 a, glm::vec2 b, glm::vec2 c)
    {
        glm::vec2 ab = b - a;
        glm::vec2 bc = c - b;
        glm::vec2 ca = a - c;

        glm::vec2 ap = p - a;
        glm::vec2 bp = p - b;
        glm::vec2 cp = p - c;

        float cross1 = Cross(ab, ap);
        float cross2 = Cross(bc, bp);
        float cross3 = Cross(ca, cp);

        if (cross1 > 0.0f || cross2 > 0.0f || cross3 > 0.0f)
        {
            return false;
        }

        return true;
    }

    bool Triangulate(std::span<glm::vec2> polygon, std::vector<uint32_t> &triangles)
    {
        std::vector<uint32_t> indexList;
        for (size_t i = 0; i < polygon.size(); i++)
        {
            indexList.push_back(i);
        }

        while (indexList.size() > 3)
        {
            for (size_t i = 0; i < indexList.size(); i++)
            {
                uint32_t a = GetItem<uint32_t>(indexList, i);
                uint32_t b = GetItem<uint32_t>(indexList, i - 1);
                uint32_t c = GetItem<uint32_t>(indexList, i + 1);

                Graphics::Triangle2D tri;
                tri.A = polygon[a];
                tri.B = polygon[b];
                tri.C = polygon[c];

                bool isEar = true;

                for (size_t j = 0; j < polygon.size(); j++)
                {
                    if (j == a || j == b || j == c)
                    {
                        continue;
                    }

                    glm::vec2 p = polygon[j];

                    if (tri.Contains(p))
                    {
                        isEar = false;
                        break;
                    }
                }

                if (isEar)
                {
                    triangles.push_back(a);
                    triangles.push_back(b);
                    triangles.push_back(c);

                    indexList.erase(indexList.begin() + i);
                    break;
                }
            }
        }

        triangles.push_back(indexList[0]);
        triangles.push_back(indexList[1]);
        triangles.push_back(indexList[2]);

        return true;
    }

    WindingOrder GetWindingOrder(glm::vec2 a, glm::vec2 b, glm::vec2 c)
    {
        return ((b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y)) < 0 ? WindingOrder::Clockwise : WindingOrder::CounterClockwise;
    }

    std::vector<glm::vec2> ReverseWindingOrder(const std::vector<glm::vec2> &vertices)
    {
        std::vector<glm::vec2> newVertices{vertices.begin(), vertices.end()};
        std::reverse(newVertices.begin(), newVertices.end());
        return newVertices;
    }
}