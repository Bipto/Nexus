#include "AssimpImporter.hpp"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

namespace Nexus
{
	Ref<Graphics::Mesh> ProcessMesh(aiMesh *mesh, const aiScene *scene, Graphics::GraphicsDevice *device)
	{
		std::vector<Graphics::VertexPositionTexCoordNormalTangentBitangent> vertices;
		std::vector<unsigned int>											indices;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Graphics::VertexPositionTexCoordNormalTangentBitangent vertex;
			vertex.Position.x = mesh->mVertices[i].x;
			vertex.Position.y = mesh->mVertices[i].y;
			vertex.Position.z = mesh->mVertices[i].z;
			vertex.Normal.x	  = mesh->mNormals[i].x;
			vertex.Normal.y	  = mesh->mNormals[i].y;
			vertex.Normal.z	  = mesh->mNormals[i].z;

			vertex.TexCoords = {
				0,
				0,
			};
			if (mesh->mTextureCoords[0])
			{
				vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
				vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
			}

			vertices.push_back(vertex);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) { indices.push_back(face.mIndices[j]); }
		}

		std::string name = std::string(mesh->mName.C_Str());

		Nexus::Graphics::BufferDescription vertexBufferDesc;
		vertexBufferDesc.Size  = vertices.size() * sizeof(Graphics::VertexPositionTexCoordNormalTangentBitangent);
		vertexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
		auto vertexBuffer	   = device->CreateVertexBuffer(vertexBufferDesc, vertices.data());

		Nexus::Graphics::BufferDescription indexBufferDesc;
		indexBufferDesc.Size  = indices.size() * sizeof(unsigned int);
		indexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
		auto indexBuffer	  = device->CreateIndexBuffer(indexBufferDesc, indices.data());

		return CreateRef<Graphics::Mesh>(vertexBuffer, indexBuffer, name);
	}

	void ProcessNode(aiNode *node, const aiScene *scene, std::vector<Ref<Graphics::Mesh>> &meshes, Graphics::GraphicsDevice *device)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(ProcessMesh(mesh, scene, device));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++) { ProcessNode(node->mChildren[i], scene, meshes, device); }
	}

	Ref<Graphics::Model> AssimpImporter::Import(const std::string &filepath, Graphics::GraphicsDevice *device)
	{
		std::vector<Ref<Graphics::Mesh>> meshes;
		Assimp::Importer				 importer = {};

		auto flags = aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_FindInvalidData |
					 aiProcess_GenSmoothNormals | aiProcess_ImproveCacheLocality | aiProcess_CalcTangentSpace | aiProcess_GenUVCoords |
					 aiProcess_ValidateDataStructure | aiProcess_FindInstances | aiProcess_GlobalScale | aiProcess_PreTransformVertices |
					 aiProcess_TransformUVCoords | aiProcess_FixInfacingNormals;

		const aiScene *scene = importer.ReadFile(filepath, flags);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::string errorMessage = "Assimp Error:";
			errorMessage += importer.GetErrorString();
			NX_ERROR(errorMessage.c_str());
			return {};
		}

		ProcessNode(scene->mRootNode, scene, meshes, device);
		return CreateRef<Graphics::Model>(meshes);
	}
}	 // namespace Nexus