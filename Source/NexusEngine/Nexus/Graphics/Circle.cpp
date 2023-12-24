#include "Circle.hpp"

namespace Nexus::Graphics
{
    Circle::Circle(const glm::vec2 &position, float radius)
        : m_Position(position), m_Radius(radius)
    {
    }
    const glm::vec2 &Circle::GetPosition() const
    {
        return m_Position;
    }
    float Circle::GetRadius() const
    {
        return m_Radius;
    }

    void Circle::SetPosition(const glm::vec2 &position)
    {
        m_Position = position;
    }

    void Circle::SetRadius(float radius)
    {
        m_Radius = radius;
    }

    bool Circle::ContainsPoint(Nexus::Point<int> point)
    {
        if ((point.X - m_Position.x) * (point.X - m_Position.x) +
                (point.Y - m_Position.y) * (point.Y - m_Position.y) <=
            m_Radius * m_Radius)
        {
            return true;
        }

        return false;
    }
}