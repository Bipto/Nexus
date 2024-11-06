#include "Vertex.hpp"

namespace Nexus::Graphics
{
	VertexBufferElement::VertexBufferElement(ShaderDataType type, const std::string &name)
		: Name(name),
		  Type(type),
		  Size(GetShaderDataTypeSize(type)),
		  Offset(0)
	{
	}

	uint32_t VertexBufferElement::GetComponentCount() const
	{
		switch (Type)
		{
			case ShaderDataType::Byte: return 1; break;
			case ShaderDataType::Byte2: return 1 * 2; break;
			case ShaderDataType::Byte4: return 1 * 4; break;
			case ShaderDataType::NormByte: return 1; break;
			case ShaderDataType::NormByte2: return 1 * 2; break;
			case ShaderDataType::NormByte4: return 1 * 4; break;
			case ShaderDataType::Float: return 1; break;
			case ShaderDataType::Float2: return 1 * 2; break;
			case ShaderDataType::Float3: return 1 * 3; break;
			case ShaderDataType::Float4: return 1 * 4; break;
			case ShaderDataType::Half: return 1; break;
			case ShaderDataType::Half2: return 1 * 2; break;
			case ShaderDataType::Half4: return 1 * 4; break;
			case ShaderDataType::Int: return 1; break;
			case ShaderDataType::Int2: return 1 * 2; break;
			case ShaderDataType::Int3: return 1 * 3; break;
			case ShaderDataType::Int4: return 1 * 4; break;
			case ShaderDataType::SignedByte: return 1; break;
			case ShaderDataType::SignedByte2: return 1 * 2; break;
			case ShaderDataType::SignedByte4: return 1 * 4; break;
			case ShaderDataType::SignedByteNormalized: return 1; break;
			case ShaderDataType::SignedByte2Normalized: return 1 * 2; break;
			case ShaderDataType::SignedByte4Normalized: return 1 * 4; break;
			case ShaderDataType::Short: return 1; break;
			case ShaderDataType::ShortNormalized: return 1; break;
			case ShaderDataType::Short2: return 1 * 2; break;
			case ShaderDataType::Short2Normalized: return 1 * 2; break;
			case ShaderDataType::Short4: return 1 * 4; break;
			case ShaderDataType::Short4Normalized: return 1 * 4; break;
			case ShaderDataType::UInt: return 1; break;
			case ShaderDataType::UInt2: return 1 * 2; break;
			case ShaderDataType::UInt3: return 1 * 3; break;
			case ShaderDataType::UInt4: return 1 * 4; break;
			case ShaderDataType::UShort: return 1; break;
			case ShaderDataType::UShort2: return 1 * 2; break;
			case ShaderDataType::UShort4: return 1 * 4; break;
			case ShaderDataType::UShortNormalized: return 1; break;
			case ShaderDataType::UShort2Normalized: return 1 * 2; break;
			case ShaderDataType::UShort4Normalized: return 1 * 4; break;
			default: return 0; break;
		}
	}

	bool VertexBufferLayout::IsVertexBuffer() const
	{
		return m_InstanceStepRate == 0;
	}

	bool VertexBufferLayout::IsInstanceBuffer() const
	{
		return m_InstanceStepRate != 0;
	}

	uint32_t VertexBufferLayout::GetInstanceStepRate() const
	{
		return m_InstanceStepRate;
	}

	void VertexBufferLayout::CalculateOffsetsAndStride()
	{
		size_t offset = 0;
		m_Stride	  = 0;
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
		validIndex		= validIndex % arraySize;
		return validIndex;
	}

	std::vector<VertexPositionTexCoordNormalTangentBitangent> Utilities::GenerateTangentAndBinormals(
		const std::vector<VertexPositionTexCoordNormal> &vertices)
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

			float	  r			= 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			glm::vec3 tangent	= (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
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
}	 // namespace Nexus::Graphics