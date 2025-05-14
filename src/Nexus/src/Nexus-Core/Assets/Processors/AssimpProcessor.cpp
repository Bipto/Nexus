#include "Nexus-Core/Assets/Processors/AssimpProcessor.hpp"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "Nexus-Core/Runtime/Project.hpp"

#include "stb_image.h"

namespace Nexus::Processors
{
	void ProcessMesh(aiMesh *mesh, const aiScene *scene, std::vector<Graphics::MeshData> &meshes, Graphics::GraphicsDevice *device)
	{
		Graphics::MeshData meshData = {};

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

			meshData.vertices.push_back(vertex);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) { meshData.indices.push_back(face.mIndices[j]); }
		}

		meshData.name		   = std::string(mesh->mName.C_Str());
		meshData.materialIndex = mesh->mMaterialIndex;
		meshes.push_back(meshData);
	}

	void ProcessNode(aiNode										  *node,
					 const aiScene								  *scene,
					 const std::vector<Nexus::Graphics::Material> &materials,
					 std::vector<Graphics::MeshData>			  &meshData,
					 Graphics::GraphicsDevice					  *device)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			ProcessMesh(mesh, scene, meshData, device);
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++) { ProcessNode(node->mChildren[i], scene, materials, meshData, device); }
	}

	Nexus::Ref<Nexus::Graphics::Texture> LoadEmbeddedTexture(const aiTexture *texture, Nexus::Graphics::GraphicsDevice *device)
	{
		std::vector<unsigned char> pixels;
		pixels.reserve(texture->mWidth * texture->mHeight * 4);
		memcpy(pixels.data(), texture->pcData, pixels.size());
		Nexus::Utils::FlipPixelsVertically(pixels.data(), texture->mWidth, texture->mHeight, Graphics::PixelFormat::R8_G8_B8_A8_UNorm);

		Nexus::Graphics::TextureSpecification textureSpec = {};
		textureSpec.Format								  = Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm;
		textureSpec.MipLevels							  = 1;
		textureSpec.ArrayLayers							  = 1;
		textureSpec.Width								  = texture->mWidth;
		textureSpec.Height								  = texture->mHeight;
		textureSpec.Type								  = Nexus::Graphics::TextureType::Texture2D;
		textureSpec.Usage								  = Nexus::Graphics::TextureUsage_Sampled;

		Nexus::Ref<Nexus::Graphics::Texture> createdTexture = Nexus::Ref<Nexus::Graphics::Texture>(device->CreateTexture(textureSpec));

		Graphics::DeviceBufferDescription bufferDesc   = {};
		bufferDesc.Type								   = Graphics::DeviceBufferType::Upload;
		bufferDesc.SizeInBytes						   = pixels.size();
		bufferDesc.StrideInBytes					   = pixels.size();
		Ref<Graphics::DeviceBuffer> buffer			   = device->CreateDeviceBuffer(bufferDesc);

		Ref<Graphics::CommandList> cmdList = device->CreateCommandList();

		return createdTexture;
	}

	Nexus::Ref<Nexus::Graphics::Texture> LoadTextureFile(const std::string				 &filename,
														 const std::string				 &directory,
														 bool							  generateMips,
														 Nexus::Graphics::GraphicsDevice *device)
	{
		std::string texturePath = directory + std::string("/") + filename;

		Nexus::Ref<Nexus::Graphics::Texture> texture = nullptr;

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

			Nexus::Ref<Nexus::Graphics::Texture> diffuseTexture	 = nullptr;
			Nexus::Ref<Nexus::Graphics::Texture> normalTexture	 = nullptr;
			Nexus::Ref<Nexus::Graphics::Texture> specularTexture = nullptr;

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

	void WriteBinaryModelFile(const std::string &filepath, const std::vector<Graphics::MeshData> &meshes)
	{
		std::ofstream file(filepath, std::ios::binary);

		file << "MODEL ";
		file << meshes.size() << " ";

		for (size_t i = 0; i < meshes.size(); i++)
		{
			const Graphics::MeshData &data				= meshes[i];
			size_t					  vertexSizeInBytes = data.vertices.size() * sizeof(data.vertices[0]);
			size_t					  indexSizeInBytes	= data.indices.size() * sizeof(data.indices[0]);

			file << vertexSizeInBytes << " " << indexSizeInBytes << " ";

			file.write((const char *)data.vertices.data(), vertexSizeInBytes);
			file.write((const char *)data.indices.data(), indexSizeInBytes);
		}

		file.close();
	}

	ModelImportData AssimpProcessor::LoadModel(const std::string &filepath, Graphics::GraphicsDevice *device)
	{
		Assimp::Importer	  importer = {};
		std::filesystem::path path	   = filepath;

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

		std::vector<Graphics::Material> materials = ImportMaterials(scene, path.parent_path().string(), device);
		std::vector<Graphics::MeshData> meshData  = {};

		ProcessNode(scene->mRootNode, scene, materials, meshData, device);

		return ModelImportData {.meshes = meshData, .materials = materials};
	}

	Ref<Graphics::Model> AssimpProcessor::Import(const std::string &filepath, Graphics::GraphicsDevice *device)
	{
		ModelImportData					 importData = LoadModel(filepath, device);
		std::vector<Ref<Graphics::Mesh>> meshes;

		for (size_t i = 0; i < importData.meshes.size(); i++)
		{
			const Graphics::MeshData &data = importData.meshes[i];

			Nexus::Ref<Nexus::Graphics::DeviceBuffer> vertexBuffer =
				Nexus::Utils::CreateFilledVertexBuffer(data.vertices.data(),
													   data.vertices.size() * sizeof(data.vertices[0]),
													   sizeof(data.vertices[0]),
													   device);

			Nexus::Ref<Nexus::Graphics::DeviceBuffer> indexBuffer =
				Nexus::Utils::CreateFilledIndexBuffer(data.indices.data(),
													  data.indices.size() * sizeof(data.indices[0]),
													  sizeof(data.indices[0]),
													  device);

			Graphics::Material		   material = importData.materials[data.materialIndex];
			Nexus::Ref<Graphics::Mesh> mesh		= CreateRef<Graphics::Mesh>(vertexBuffer, indexBuffer, material, data.name);
			meshes.push_back(mesh);
		}

		return CreateRef<Graphics::Model>(meshes);
	}

	GUID AssimpProcessor::Process(const std::string &filepath, Graphics::GraphicsDevice *device, Project *project)
	{
		ModelImportData importData = LoadModel(filepath, device);

		std::filesystem::path path			 = filepath;
		std::filesystem::path assetPath		 = path.stem().string() + std::string(".model");
		std::filesystem::path outputFilePath = project->GetFullAssetsDirectory() + "/" + assetPath.string();

		WriteBinaryModelFile(outputFilePath.string(), importData.meshes);

		Assets::AssetRegistry &registry = project->GetAssetRegistry();
		return registry.RegisterAsset(GetName(), assetPath.string());
	}

	std::any Nexus::Processors::AssimpProcessor::Import(const std::string &filepath)
	{
		return nullptr;
	}
}	 // namespace Nexus::Processors
