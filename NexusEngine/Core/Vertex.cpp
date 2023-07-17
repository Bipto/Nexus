#include "Vertex.hpp"

namespace Nexus::Graphics
{
    VertexBufferElement::VertexBufferElement(ShaderDataType type, const std::string &name, bool normalized)
        : Name(name), Type(type), Size(GetShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
    {
    }

    uint32_t VertexBufferElement::GetComponentCount() const
    {
        switch (Type)
        {
        case ShaderDataType::Float:
            return 1;
        case ShaderDataType::Float2:
            return 2;
        case ShaderDataType::Float3:
            return 3;
        case ShaderDataType::Float4:
            return 4;
        case ShaderDataType::Int:
            return 1;
        case ShaderDataType::Int2:
            return 2;
        case ShaderDataType::Int3:
            return 3;
        case ShaderDataType::Int4:
            return 4;
        }
    }

    void VertexBufferLayout::CalculateOffsetsAndStride()
    {
        size_t offset = 0;
        m_Stride = 0;
        for (auto &element : m_Elements)
        {
            element.Offset = offset;
            offset += element.Size;
            m_Stride += element.Size;
        }
    }

    int GetIndexInArray(int index, int arraySize)
    {
        auto validIndex = index + 1;
        validIndex = validIndex % arraySize;
        return validIndex;
    }

    std::vector<VertexPositionTexCoordNormalTangentBitangent> Utilities::GenerateTangentAndBinormals(const std::vector<VertexPositionTexCoordNormal> &vertices)
    {
        if (vertices.size() % 3 != 0)
        {
            throw std::runtime_error("Attempting to generate normals from a mesh not made of triangles");
        }

        std::vector<VertexPositionTexCoordNormalTangentBitangent> output;

        for (int i = 0; i < vertices.size(); i += 3)
        {
            glm::vec3 pos1 = vertices[i + 0].Position;
            glm::vec3 pos2 = vertices[i + 1].Position;
            glm::vec3 pos3 = vertices[i + 2].Position;

            glm::vec2 uv1 = vertices[i + 0].TexCoords;
            glm::vec2 uv2 = vertices[i + 1].TexCoords;
            glm::vec2 uv3 = vertices[i + 2].TexCoords;

            glm::vec3 normal1 = vertices[i + 0].Normal;
            glm::vec3 normal2 = vertices[i + 1].Normal;
            glm::vec3 normal3 = vertices[i + 2].Normal;

            glm::vec3 deltaPos1 = pos2 - pos1;
            glm::vec3 deltaPos2 = pos3 - pos2;

            glm::vec2 deltaUV1 = uv2 - uv1;
            glm::vec2 deltaUV2 = uv3 - uv1;

            float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
            glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
            glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

            VertexPositionTexCoordNormalTangentBitangent v1(pos1, uv1, normal1, tangent, bitangent);
            VertexPositionTexCoordNormalTangentBitangent v2(pos2, uv2, normal2, tangent, bitangent);
            VertexPositionTexCoordNormalTangentBitangent v3(pos3, uv3, normal3, tangent, bitangent);

            output.push_back(v1);
            output.push_back(v2);
            output.push_back(v3);
        }

        return output;
    }
}