#pragma once

#include "Nexus-Core/nxpch.hpp"
#include "Nexus-Core/Point.hpp"

namespace Nexus::Graphics
{
    template <typename T>
    class CatmullRom
    {
    public:
        CatmullRom() = default;

        Point2D<T> GetPoint(T t) const
        {
            size_t p0, p1, p2, p3;

            if (!m_Looped)
            {
                p1 = (size_t)t + 1;
                p2 = p1 + 1;
                p3 = p2 + 1;
                p0 = p1 - 1;
            }
            else
            {
                p1 = (size_t)t;
                p2 = (p1 + 1) % m_Points.size();
                p3 = (p2 + 1) % m_Points.size();
                p0 = p1 >= 1 ? p1 - 1 : m_Points.size() - 1;
            }

            t = t - (size_t)t;

            T tt = t * t;
            T ttt = tt * t;

            T q1 = -ttt + 2 * tt - t;
            T q2 = 3 * ttt - 5 * tt + 2;
            T q3 = -3 * ttt + 4 * tt + t;
            T q4 = ttt - tt;

            T tx = (m_Points[p0].X * q1 + m_Points[p1].X * q2 + m_Points[p2].X * q3 + m_Points[p3].X * q4) / 2;
            T ty = (m_Points[p0].Y * q1 + m_Points[p1].Y * q2 + m_Points[p2].Y * q3 + m_Points[p3].Y * q4) / 2;

            return {tx, ty};
        }

        void SetLooped(bool looped)
        {
            m_Looped = looped;
        }

        bool IsLooped() const
        {
            return m_Looped;
        }

        void SetPoints(const std::vector<Point2D<T>> &points)
        {
            m_Points = points;
        }

        const std::vector<Point2D<T>> &GetPoints() const
        {
            return m_Points;
        }

        const size_t GetNumberOfPoints() const
        {
            return m_Points.size();
        }

    private:
        std::vector<Point2D<T>> m_Points;
        bool m_Looped = false;
    };
}
