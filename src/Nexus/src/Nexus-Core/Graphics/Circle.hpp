#pragma once

#include "Nexus-Core/Point.hpp"

namespace Nexus::Graphics
{
	template<typename T>
	class Circle
	{
	  public:
		Circle() = default;

		Circle(const Point2D<T> &position, T radius)
		{
			m_Position = position;
			m_Radius   = radius;
		}

		const Point2D<T> &GetPosition() const
		{
			return m_Position;
		}

		T GetRadius() const
		{
			return m_Radius;
		}

		void SetPosition(const Point2D<T> &position)
		{
			m_Position = position;
		}

		void SetRadius(T radius)
		{
			m_Radius = radius;
		}

		bool Contains(const Nexus::Point2D<T> &point) const
		{
			if ((point.X - m_Position.X) * (point.X - m_Position.X) + (point.Y - m_Position.Y) * (point.Y - m_Position.Y) <= m_Radius * m_Radius)
			{
				return true;
			}

			return false;
		}

		template<typename Other>
		const Circle<Other> To() const
		{
			return Circle<Other>(Nexus::Point2D<Other>((Other)m_Position.X, (Other)m_Position.Y), (Other)m_Radius);
		}

		void Deconstruct(T *x, T *y, T *radius) const
		{
			if (x)
			{
				*x = m_Position.X;
			}

			if (y)
			{
				*y = m_Position.Y;
			}

			if (radius)
			{
				*radius = m_Radius;
			}
		}

	  private:
		Point2D<T> m_Position = {0, 0};
		T		   m_Radius	  = 0;
	};
}	 // namespace Nexus::Graphics