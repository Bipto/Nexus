#pragma once

#include "Mesh.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	class Model
	{
	  public:
		NX_API Model(std::vector<Ref<Mesh>> meshes) : m_Meshes(meshes)
		{
		}

		NX_API Model()
		{
		}

		NX_API const std::vector<Ref<Mesh>> &GetMeshes()
		{
			return m_Meshes;
		}

	  private:
		std::vector<Ref<Mesh>> m_Meshes;
	};
};	  // namespace Nexus::Graphics