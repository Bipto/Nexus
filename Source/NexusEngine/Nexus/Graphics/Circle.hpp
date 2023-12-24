#pragma once

#include "Nexus/Point.hpp"

#include "glm/glm.hpp"

namespace Nexus::Graphics
{
    class Circle
    {
    public:
        Circle() = default;
        Circle(const glm::vec2 &position, float radius);
        const glm::vec2 &GetPosition() const;
        float GetRadius() const;
        void SetPosition(const glm::vec2 &position);
        void SetRadius(float radius);

        bool ContainsPoint(Nexus::Point<int> point);

    private:
        glm::vec2 m_Position = {0.0f, 0.0f};
        float m_Radius = 0;
    };
}