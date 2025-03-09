#include "Nexus-Core/Graphics/Polygon.hpp"

#include "Nexus-Core/Utils/Utils.hpp"

namespace Nexus::Graphics
{
	Polygon::Polygon(const std::vector<Triangle2D> &triangles) : m_Triangles(triangles)
	{
	}

	void Polygon::SetTriangles(const std::vector<Triangle2D> &triangles)
	{
		m_Triangles = triangles;
	}

	const std::vector<Triangle2D> &Polygon::GetTriangles() const
	{
		return m_Triangles;
	}
}	 // namespace Nexus::Graphics