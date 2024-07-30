#include "MeshFactory.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
    Ref<Mesh> MeshFactory::CreateCube()
    {
        std::vector<VertexPositionTexCoordNormal> vertices =
            {
                // FRONT
                {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}, // 0  - FRONT TOP LEFT
                {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},  // 1  - FRONT TOP RIGHT
                {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},   // 2  - FRONT BOTTOM RIGHT
                {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},  // 3  - FRONT BOTTOM LEFT

                // BACK
                {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}}, // 4  - BACK TOP LEFT
                {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},  // 5  - BACK TOP RIGHT
                {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},   // 6  - BACK BOTTOM RIGHT
                {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},  // 7  - BACK BOTTOM LEFT

                // TOP
                {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // 8  - TOP BACK LEFT
                {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // 9  - TOP BACK RIGHT
                {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},   // 10 - TOP FRONT RIGHT
                {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},  // 11 - TOP FRONT LEFT

                // BOTTOM
                {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}}, // 12  - BOTTOM BACK LEFT
                {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},  // 13  - BOTTOM BACK RIGHT
                {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},   // 14 - BOTTOM FRONT RIGHT
                {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},  // 15 - BOTTOM FRONT LEFT

                // LEFT
                {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}}, // 16 - LEFT BACK TOP
                {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},  // 17 - LEFT FRONT TOP
                {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},   // 18 - LEFT FRONT BOTTOM
                {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},  // 19 - LEFT BACK BOTTOM

                // RIGHT
                {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}}, // 20 - RIGHT BACK TOP
                {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},  // 21 - RIGHT FRONT TOP
                {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},   // 22 - RIGHT FRONT BOTTOM
                {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}}   // 23 - RIGHT BACK BOTTOM
            };

        std::vector<unsigned int> indices =
            {
                0, 1, 2, // FRONT
                2, 3, 0,

                6, 5, 4, // BACK
                4, 7, 6,

                8, 9, 10, // TOP
                10, 11, 8,

                14, 13, 12, // BOTTOM
                12, 15, 14,

                16, 17, 18, // LEFT
                18, 19, 16,

                22, 21, 20,
                20, 23, 22};

        auto fullVertices = Nexus::Graphics::Utilities::GenerateTangentAndBinormals(vertices);

        Nexus::Graphics::BufferDescription vertexBufferDesc;
        vertexBufferDesc.Size = vertices.size() * sizeof(VertexPositionTexCoordNormalTangentBitangent);
        vertexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
        auto vertexBuffer = m_Device->CreateVertexBuffer(vertexBufferDesc, fullVertices.data());

        Nexus::Graphics::BufferDescription indexBufferDesc;
        indexBufferDesc.Size = indices.size() * sizeof(unsigned int);
        indexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
        auto indexBuffer = m_Device->CreateIndexBuffer(indexBufferDesc, indices.data());

        return CreateRef<Mesh>(vertexBuffer, indexBuffer);
    }

    Ref<Mesh> MeshFactory::CreateSprite()
    {
        std::vector<VertexPositionTexCoordNormalTangentBitangent> vertices =
            {
                {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1, 1, 1}, {1, 1, 1}},
                {{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1, 1, 1}, {1, 1, 1}},
                {{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1, 1, 1}, {1, 1, 1}},
                {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1, 1, 1}, {1, 1, 1}}};

        std::vector<unsigned int> indices =
            {
                0, 1, 2,
                2, 3, 0};

        Nexus::Graphics::BufferDescription vertexBufferDesc;
        vertexBufferDesc.Size = vertices.size() * sizeof(VertexPositionTexCoordNormalTangentBitangent);
        vertexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
        auto vertexBuffer = m_Device->CreateVertexBuffer(vertexBufferDesc, vertices.data());

        Nexus::Graphics::BufferDescription indexBufferDesc;
        indexBufferDesc.Size = indices.size() * sizeof(unsigned int);
        indexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
        auto indexBuffer = m_Device->CreateIndexBuffer(indexBufferDesc, indices.data());

        return CreateRef<Mesh>(vertexBuffer, indexBuffer);
    }

    Ref<Mesh> MeshFactory::CreateTriangle()
    {
        std::vector<VertexPositionTexCoordNormalTangentBitangent> vertices =
            {
                {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1, 1, 1}, {1, 1, 1}},
                {{0.0f, 0.5f, 0.0f}, {0.5f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1, 1, 1}, {1, 1, 1}},
                {{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1, 1, 1}, {1, 1, 1}}};

        std::vector<unsigned int> indices = {
            0, 1, 2};

        Nexus::Graphics::BufferDescription vertexBufferDesc;
        vertexBufferDesc.Size = vertices.size() * sizeof(VertexPositionTexCoordNormalTangentBitangent);
        vertexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
        auto vertexBuffer = m_Device->CreateVertexBuffer(vertexBufferDesc, vertices.data());

        Nexus::Graphics::BufferDescription indexBufferDesc;
        indexBufferDesc.Size = indices.size() * sizeof(unsigned int);
        indexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
        auto indexBuffer = m_Device->CreateIndexBuffer(indexBufferDesc, indices.data());

        return CreateRef<Mesh>(vertexBuffer, indexBuffer);
    }

    Ref<Mesh> ProcessMesh(aiMesh *mesh, const aiScene *scene, GraphicsDevice *device)
    {
        std::vector<VertexPositionTexCoordNormalTangentBitangent> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            VertexPositionTexCoordNormalTangentBitangent vertex;
            vertex.Position.x = mesh->mVertices[i].x;
            vertex.Position.y = mesh->mVertices[i].y;
            vertex.Position.z = mesh->mVertices[i].z;
            vertex.Normal.x = mesh->mNormals[i].x;
            vertex.Normal.y = mesh->mNormals[i].y;
            vertex.Normal.z = mesh->mNormals[i].z;

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
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        std::string name = std::string(mesh->mName.C_Str());

        Nexus::Graphics::BufferDescription vertexBufferDesc;
        vertexBufferDesc.Size = vertices.size() * sizeof(VertexPositionTexCoordNormalTangentBitangent);
        vertexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
        auto vertexBuffer = device->CreateVertexBuffer(vertexBufferDesc, vertices.data());

        Nexus::Graphics::BufferDescription indexBufferDesc;
        indexBufferDesc.Size = indices.size() * sizeof(unsigned int);
        indexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
        auto indexBuffer = device->CreateIndexBuffer(indexBufferDesc, indices.data());

        return CreateRef<Mesh>(vertexBuffer, indexBuffer, name);
    }

    void ProcessNode(aiNode *node, const aiScene *scene, std::vector<Ref<Mesh>> &meshes, GraphicsDevice *device)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(ProcessMesh(mesh, scene, device));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene, meshes, device);
        }
    }

    Ref<Model> MeshFactory::CreateFrom3DModelFile(const std::string &filepath)
    {
        std::vector<Ref<Mesh>> meshes;
        Assimp::Importer importer;
        auto flags =
            aiProcess_FlipUVs |
            aiProcess_JoinIdenticalVertices |
            aiProcess_Triangulate |
            aiProcess_FindInvalidData |
            aiProcess_GenSmoothNormals |
            aiProcess_ImproveCacheLocality |
            aiProcess_CalcTangentSpace |
            aiProcess_GenUVCoords |
            aiProcess_ValidateDataStructure |
            aiProcess_FindInstances |
            aiProcess_GlobalScale |
            aiProcess_PreTransformVertices |
            aiProcess_TransformUVCoords |
            aiProcess_FixInfacingNormals;

        const aiScene *scene = importer.ReadFile(filepath, flags);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "Assimp error: " << importer.GetErrorString() << std::endl;
        }
        ProcessNode(scene->mRootNode, scene, meshes, m_Device);
        return CreateRef<Model>(meshes);
    }
}