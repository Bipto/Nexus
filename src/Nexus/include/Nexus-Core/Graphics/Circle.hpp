#pragma once

#include "Nexus-Core/Point.hpp"

namespace Nexus::Graphics
{
	template<typename T>
	class Circle
	{
	  public:
		NX_API Circle() = default;

		NX_API Circle(const Point2D<T> &position, T radius)
		{
			m_Position = position;
			m_Radius   = radius;
		}

		NX_API const Point2D<T> &GetPosition() const
		{
			return m_Position;
		}

		NX_API T GetRadius() const
		{
			return m_Radius;
		}

		NX_API void SetPosition(const Point2D<T> &position)
		{
			m_Position = position;
		}

		NX_API void SetRadius(T radius)
		{
			m_Radius = radius;
		}

		NX_API bool Contains(const Nexus::Point2D<T> &point) const
		{
			if ((point.X - m_Position.X) * (point.X - m_Position.X) + (point.Y - m_Position.Y) * (point.Y - m_Position.Y) <= m_Radius * m_Radius)
			{
				return true;
			}

			return false;
		}

		template<typename Other>
		NX_API const Circle<Other> To() const
		{
			return Circle<Other>(Nexus::Point2D<Other>((Other)m_Position.X, (Other)m_Position.Y), (Other)m_Radius);
		}

		NX_API void Deconstruct(T *x, T *y, T *radius) const
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