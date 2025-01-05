#include "AssimpImporter.hpp"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

namespace Nexus
{
	Ref<Graphics::Mesh> ProcessMesh(aiMesh										 *mesh,
									const aiScene								 *scene,
									const std::vector<Nexus::Graphics::Material> &materials,
									Graphics::GraphicsDevice					 *device)
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

		Graphics::Material mat = materials[mesh->mMaterialIndex];

		return CreateRef<Graphics::Mesh>(vertexBuffer, indexBuffer, mat, name);
	}

	void ProcessNode(aiNode										  *node,
					 const aiScene								  *scene,
					 std::vector<Ref<Graphics::Mesh>>			  &meshes,
					 const std::vector<Nexus::Graphics::Material> &materials,
					 Graphics::GraphicsDevice					  *device)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(ProcessMesh(mesh, scene, materials, device));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++) { ProcessNode(node->mChildren[i], scene, meshes, materials, device); }
	}

	Nexus::Ref<Nexus::Graphics::Texture2D> LoadTexture(aiMaterial					   *material,
													   aiTextureType					type,
													   int								index,
													   const std::string			   &directory,
													   bool								generateMips,
													   Nexus::Graphics::GraphicsDevice *device)
	{
		aiString textureName;
		material->GetTexture(type, index, &textureName);
		std::string texturePath = directory + std::string("/") + std::string(textureName.data);

		Nexus::Ref<Nexus::Graphics::Texture2D> texture = nullptr;

		if (std::filesystem::exists(texturePath))
		{
			texture = device->CreateTexture2D(texturePath, generateMips);
		}

		return texture;
	}

	std::vector<Nexus::Graphics::Material> ImportMaterials(const aiScene				   *scene,
														   const std::string			   &directory,
														   Nexus::Graphics::GraphicsDevice *device)
	{
		std::vector<Nexus::Graphics::Material> materials;
		materials.reserve(scene->mNumMaterials);

		for (uint32_t i = 0; i < scene->mNumMaterials; i++)
		{
			aiMaterial							  *material = scene->mMaterials[i];
			Nexus::Ref<Nexus::Graphics::Texture2D> diffuse	= LoadTexture(material, aiTextureType_DIFFUSE, 0, directory, true, device);
			Nexus::Ref<Nexus::Graphics::Texture2D> normal	= LoadTexture(material, aiTextureType_NORMALS, 0, directory, true, device);
			Nexus::Ref<Nexus::Graphics::Texture2D> specular = LoadTexture(material, aiTextureType_SPECULAR, 0, directory, true, device);

			Nexus::Graphics::Material mat {.DiffuseTexture = diffuse, .NormalTexture = normal, .SpecularTexture = specular};

			materials.push_back(mat);
		}

		return materials;
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

		std::filesystem::path			path	  = filepath;
		std::vector<Graphics::Material> materials = ImportMaterials(scene, path.parent_path().string(), device);

		ProcessNode(scene->mRootNode, scene, meshes, materials, device);
		return CreateRef<Graphics::Model>(meshes);
	}
}	 // namespace Nexus