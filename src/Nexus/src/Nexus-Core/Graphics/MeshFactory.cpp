#include "Nexus-Core/Graphics/MeshFactory.hpp"

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Importers/AssimpImporter.hpp"

namespace Nexus::Graphics
{
	Ref<Mesh> MeshFactory::CreateCube()
	{
		std::vector<VertexPositionTexCoordNormal> vertices = {
			// FRONT
			{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},	 // 0  - FRONT TOP LEFT
			{{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},	 // 1  - FRONT TOP RIGHT
			{{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},		 // 2  - FRONT BOTTOM RIGHT
			{{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},	 // 3  - FRONT BOTTOM LEFT

			// BACK
			{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},	   // 4  - BACK TOP LEFT
			{{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},	   // 5  - BACK TOP RIGHT
			{{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},	   // 6  - BACK BOTTOM RIGHT
			{{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},	   // 7  - BACK BOTTOM LEFT

			// TOP
			{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},	  // 8  - TOP BACK LEFT
			{{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},	  // 9  - TOP BACK RIGHT
			{{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},	  // 10 - TOP FRONT RIGHT
			{{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},	  // 11 - TOP FRONT LEFT

			// BOTTOM
			{{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},	  // 12  - BOTTOM BACK LEFT
			{{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},	  // 13  - BOTTOM BACK RIGHT
			{{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},	  // 14 - BOTTOM FRONT RIGHT
			{{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},	  // 15 - BOTTOM FRONT LEFT

			// LEFT
			{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},	   // 16 - LEFT BACK TOP
			{{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},	   // 17 - LEFT FRONT TOP
			{{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},	   // 18 - LEFT FRONT BOTTOM
			{{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},	   // 19 - LEFT BACK BOTTOM

			// RIGHT
			{{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},	 // 20 - RIGHT BACK TOP
			{{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},	 // 21 - RIGHT FRONT TOP
			{{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},		 // 22 - RIGHT FRONT BOTTOM
			{{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}}		 // 23 - RIGHT BACK BOTTOM
		};

		std::vector<unsigned int> indices = {0,	 1,	 2,	   // FRONT
											 2,	 3,	 0,

											 6,	 5,	 4,	   // BACK
											 4,	 7,	 6,

											 8,	 9,	 10,	// TOP
											 10, 11, 8,

											 14, 13, 12,	// BOTTOM
											 12, 15, 14,

											 16, 17, 18,	// LEFT
											 18, 19, 16,

											 22, 21, 20, 20, 23, 22};

		auto fullVertices = Nexus::Graphics::Utilities::GenerateTangentAndBinormals(vertices);

		Nexus::Graphics::BufferDescription vertexBufferDesc;
		vertexBufferDesc.Size  = vertices.size() * sizeof(VertexPositionTexCoordNormalTangentBitangent);
		vertexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
		auto vertexBuffer	   = m_Device->CreateVertexBuffer(vertexBufferDesc, fullVertices.data());

		Nexus::Graphics::BufferDescription indexBufferDesc;
		indexBufferDesc.Size  = indices.size() * sizeof(unsigned int);
		indexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
		auto indexBuffer	  = m_Device->CreateIndexBuffer(indexBufferDesc, indices.data());

		return CreateRef<Mesh>(vertexBuffer, indexBuffer, Material {});
	}

	Ref<Mesh> MeshFactory::CreateSprite()
	{
		std::vector<VertexPositionTexCoordNormalTangentBitangent> vertices = {
			{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1, 1, 1}, {1, 1, 1}},
			{{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1, 1, 1}, {1, 1, 1}},
			{{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1, 1, 1}, {1, 1, 1}},
			{{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1, 1, 1}, {1, 1, 1}}};

		std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};

		Nexus::Graphics::BufferDescription vertexBufferDesc;
		vertexBufferDesc.Size  = vertices.size() * sizeof(VertexPositionTexCoordNormalTangentBitangent);
		vertexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
		auto vertexBuffer	   = m_Device->CreateVertexBuffer(vertexBufferDesc, vertices.data());

		Nexus::Graphics::BufferDescription indexBufferDesc;
		indexBufferDesc.Size  = indices.size() * sizeof(unsigned int);
		indexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
		auto indexBuffer	  = m_Device->CreateIndexBuffer(indexBufferDesc, indices.data());

		return CreateRef<Mesh>(vertexBuffer, indexBuffer, Material {});
	}

	Ref<Mesh> MeshFactory::CreateTriangle()
	{
		std::vector<VertexPositionTexCoordNormalTangentBitangent> vertices = {
			{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1, 1, 1}, {1, 1, 1}},
			{{0.0f, 0.5f, 0.0f}, {0.5f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1, 1, 1}, {1, 1, 1}},
			{{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1, 1, 1}, {1, 1, 1}}};

		std::vector<unsigned int> indices = {0, 1, 2};

		Nexus::Graphics::BufferDescription vertexBufferDesc;
		vertexBufferDesc.Size  = vertices.size() * sizeof(VertexPositionTexCoordNormalTangentBitangent);
		vertexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
		auto vertexBuffer	   = m_Device->CreateVertexBuffer(vertexBufferDesc, vertices.data());

		Nexus::Graphics::BufferDescription indexBufferDesc;
		indexBufferDesc.Size  = indices.size() * sizeof(unsigned int);
		indexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
		auto indexBuffer	  = m_Device->CreateIndexBuffer(indexBufferDesc, indices.data());

		return CreateRef<Mesh>(vertexBuffer, indexBuffer, Material {});
	}

	Ref<Model> MeshFactory::CreateFrom3DModelFile(const std::string &filepath)
	{
		Nexus::AssimpImporter importer {};
		Ref<Model>			  model = importer.Import(filepath, m_Device);
		return model;
	}
}	 // namespace Nexus::Graphics