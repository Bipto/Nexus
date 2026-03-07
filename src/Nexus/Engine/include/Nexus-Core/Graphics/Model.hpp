#pragma once

#include "Mesh.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	struct MeshData
	{
		std::string																  name			= "";
		uint32_t																  materialIndex = 0;
		std::vector<Graphics::VertexPositionTexCoordNormalColourTangentBitangent> vertices		= {};
		std::vector<uint32_t>													  indices		= {};
	};

	class Model
	{
	  public:
		Model(std::vector<Ref<Mesh>> meshes) : m_Meshes(meshes)
		{
		}

		Model()
		{
		}

		const std::vector<Ref<Mesh>> &GetMeshes()
		{
			return m_Meshes;
		}

	  private:
		std::vector<Ref<Mesh>> m_Meshes;
	};
};	  // namespace Nexus::Graphics