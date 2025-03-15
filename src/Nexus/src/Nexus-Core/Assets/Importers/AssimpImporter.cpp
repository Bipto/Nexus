#include "Nexus-Core/Assets/Importers/AssimpImporter.hpp"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "stb_image.h"

namespace Nexus
{
	Ref<Graphics::Mesh> ProcessMesh(aiMesh										 *mesh,
									const aiScene								 *scene,
									const std::vector<Nexus::Graphics::Material> &materials,
									Graphics::GraphicsDevice					 *device)
	{
		std::vector<Graphics::VertexPositionTexCoordNormalColourTangentBitangent> vertices;
		std::vector<unsigned int>												  indices;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Graphics::VertexPositionTexCoordNormalColourTangentBitangent vertex;
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

			vertex.Colour = {1, 1, 1, 1};
			if (mesh->HasVertexColors(0))
			{
				vertex.Colour.r = mesh->mColors[0][i].r;
				vertex.Colour.g = mesh->mColors[0][i].g;
				vertex.Colour.b = mesh->mColors[0][i].b;
				vertex.Colour.a = mesh->mColors[0][i].a;
			}

			if (mesh->HasTangentsAndBitangents())
			{
				vertex.Tangent.x = mesh->mTangents[i].x;
				vertex.Tangent.y = mesh->mTangents[i].y;
				vertex.Tangent.z = mesh->mTangents[i].z;

				vertex.Bitangent.x = mesh->mBitangents[i].x;
				vertex.Bitangent.y = mesh->mBitangents[i].y;
				vertex.Bitangent.z = mesh->mBitangents[i].z;
			}
			else
			{
				vertex.Tangent.x = 0;
				vertex.Tangent.y = 0;
				vertex.Tangent.z = 0;

				vertex.Bitangent.x = 0;
				vertex.Bitangent.y = 0;
				vertex.Bitangent.z = 0;
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
		vertexBufferDesc.Size  = vertices.size() * sizeof(Graphics::VertexPositionTexCoordNormalColourTangentBitangent);
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

	Nexus::Ref<Nexus::Graphics::Texture2D> LoadEmbeddedTexture(const aiTexture *texture, Nexus::Graphics::GraphicsDevice *device)
	{
		std::vector<unsigned char> pixels;
		pixels.reserve(texture->mWidth * texture->mHeight * 4);
		memcpy(pixels.data(), texture->pcData, pixels.size());
		Nexus::Utils::FlipPixelsVertically(pixels, texture->mWidth, texture->mHeight, 1, 4);

		Nexus::Graphics::Texture2DSpecification spec = {};
		spec.Format									 = Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UInt;
		spec.MipLevels								 = 1;
		spec.Width									 = texture->mWidth;
		spec.Height									 = texture->mHeight;

		Nexus::Ref<Nexus::Graphics::Texture2D> createdTexture = device->CreateTexture2D(spec);
		createdTexture->SetData(pixels.data(), 0, 0, 0, spec.Width, spec.Height);
		return createdTexture;
	}

	Nexus::Ref<Nexus::Graphics::Texture2D> LoadTextureFile(const std::string			   &filename,
														   const std::string			   &directory,
														   bool								generateMips,
														   Nexus::Graphics::GraphicsDevice *device)
	{
		std::string texturePath = directory + std::string("/") + filename;

		Nexus::Ref<Nexus::Graphics::Texture2D> texture = nullptr;

		if (std::filesystem::is_regular_file(texturePath))
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
			aiMaterial *material = scene->mMaterials[i];

			aiString diffuseTexturePath;
			aiString normalTexturePath;
			aiString specularTexturePath;

			glm::vec4 diffuseColour	 = {1, 1, 1, 1};
			glm::vec4 specularColour = {1, 1, 1, 1};

			bool hasDiffuseTexture	= false;
			bool hasNormalTexture	= false;
			bool hasSpecularTexture = false;

			if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), diffuseTexturePath) == AI_SUCCESS)
			{
				hasDiffuseTexture = true;
			}

			if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_NORMALS, 0), normalTexturePath) == AI_SUCCESS)
			{
				hasNormalTexture = true;
			}

			if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), specularTexturePath) == AI_SUCCESS)
			{
				hasSpecularTexture = true;
			}

			aiColor4D assimpDiffuseColour;
			if (material->Get(AI_MATKEY_COLOR_DIFFUSE, assimpDiffuseColour) == AI_SUCCESS)
			{
				diffuseColour.r = assimpDiffuseColour.r;
				diffuseColour.g = assimpDiffuseColour.g;
				diffuseColour.b = assimpDiffuseColour.b;
				diffuseColour.a = assimpDiffuseColour.a;
			}

			aiColor4D assimpSpecularColour;
			if (material->Get(AI_MATKEY_COLOR_SPECULAR, assimpSpecularColour) == AI_SUCCESS)
			{
				specularColour.r = assimpSpecularColour.r;
				specularColour.g = assimpSpecularColour.g;
				specularColour.b = assimpSpecularColour.b;
				specularColour.a = assimpSpecularColour.a;
			}

			Nexus::Ref<Nexus::Graphics::Texture2D> diffuseTexture  = nullptr;
			Nexus::Ref<Nexus::Graphics::Texture2D> normalTexture   = nullptr;
			Nexus::Ref<Nexus::Graphics::Texture2D> specularTexture = nullptr;

			if (hasDiffuseTexture)
			{
				if (auto embeddedTexture = scene->GetEmbeddedTexture(diffuseTexturePath.C_Str()))
				{
					diffuseTexture = LoadEmbeddedTexture(embeddedTexture, device);
				}
				else
				{
					diffuseTexture = LoadTextureFile(diffuseTexturePath.C_Str(), directory, true, device);
				}
			}

			if (hasNormalTexture)
			{
				if (auto embeddedTexture = scene->GetEmbeddedTexture(normalTexturePath.C_Str()))
				{
					normalTexture = LoadEmbeddedTexture(embeddedTexture, device);
				}
				else
				{
					normalTexture = LoadTextureFile(normalTexturePath.C_Str(), directory, true, device);
				}
			}

			if (hasSpecularTexture)
			{
				if (auto embeddedTexture = scene->GetEmbeddedTexture(specularTexturePath.C_Str()))
				{
					specularTexture = LoadEmbeddedTexture(embeddedTexture, device);
				}
				else
				{
					specularTexture = LoadTextureFile(specularTexturePath.C_Str(), directory, true, device);
				}
			}

			Nexus::Graphics::Material mat = {};
			mat.DiffuseTexture			  = diffuseTexture;
			mat.NormalTexture			  = normalTexture;
			mat.SpecularTexture			  = specularTexture;

			mat.DiffuseColour  = {diffuseColour.r, diffuseColour.g, diffuseColour.b, diffuseColour.a};
			mat.SpecularColour = {specularColour.r, specularColour.g, specularColour.b, specularColour.a};

			materials.push_back(mat);
		}

		return materials;
	}

	aiMatrix4x4 GlmToAssimpMat4(const glm::mat4 &matrix)
	{
		aiMatrix4x4 result;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; j++) { result[i][j] = matrix[i][j]; }
		}

		return result;
	}

	void RotateScene(aiNode *node, const glm::mat4 &rotationMatrix)
	{
		aiMatrix4x4 transform = node->mTransformation;
		aiMatrix4x4 rotation  = GlmToAssimpMat4(rotationMatrix);
		node->mTransformation = transform * rotation;

		for (unsigned int i = 0; i < node->mNumChildren; i++) { RotateScene(node->mChildren[i], rotationMatrix); }
	}

	void RotateBy180Degrees(aiScene *scene)
	{
		if (scene->mRootNode)
		{
			glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			RotateScene(scene->mRootNode, rotationMatrix);
		}
	}

	Ref<Graphics::Model> AssimpImporter::Import(const std::string &filepath, Graphics::GraphicsDevice *device)
	{
		std::vector<Ref<Graphics::Mesh>> meshes;
		Assimp::Importer				 importer = {};

		unsigned int flags = aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_FindInvalidData | aiProcess_GenSmoothNormals |
							 aiProcess_ImproveCacheLocality | aiProcess_CalcTangentSpace | aiProcess_ValidateDataStructure | aiProcess_FindInstances |
							 aiProcess_GlobalScale | aiProcess_PreTransformVertices | aiProcess_TransformUVCoords | aiProcess_FixInfacingNormals |
							 aiProcess_MakeLeftHanded;

		importer.SetPropertyBool(AI_CONFIG_IMPORT_NO_SKELETON_MESHES, true);
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_CAMERAS, false);
		importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
		importer.SetPropertyInteger(AI_CONFIG_PP_SBBC_MAX_BONES, 4);
		importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 66.0f);
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_STRICT_MODE, false);

		aiScene *scene = (aiScene *)importer.ReadFile(filepath, flags);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::string errorMessage = "Assimp Error:";
			errorMessage += importer.GetErrorString();
			NX_ERROR(errorMessage.c_str());
			return {};
		}

		// RotateBy180Degrees(scene);

		std::filesystem::path			path	  = filepath;
		std::vector<Graphics::Material> materials = ImportMaterials(scene, path.parent_path().string(), device);

		ProcessNode(scene->mRootNode, scene, meshes, materials, device);
		return CreateRef<Graphics::Model>(meshes);
	}
}	 // namespace Nexus