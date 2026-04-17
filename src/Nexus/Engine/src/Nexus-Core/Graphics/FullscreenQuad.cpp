#include "Nexus-Core/Graphics/FullscreenQuad.hpp"
#include "Nexus-Core/Utils/Utils.hpp"
#include "Nexus-Core/Vertex.hpp"

namespace Nexus::Graphics
{
	FullscreenQuad::FullscreenQuad(IGraphicsDevice *device, CommandQueueHandle commandQueue, bool hasUv)
		: m_GraphicsDevice(device),
		  m_CommandQueue(commandQueue),
		  m_HasUV(hasUv)
	{
		if (m_HasUV)
		{
			CreateWithUV();
		}
		else
		{
			CreateWithoutUV();
		}
	}

	Ref<IDeviceBuffer> FullscreenQuad::GetVertexBuffer()
	{
		return m_VertexBuffer;
	}

	Ref<IDeviceBuffer> FullscreenQuad::GetIndexBuffer()
	{
		return m_IndexBuffer;
	}

	Nexus::Graphics::VertexBufferLayout FullscreenQuad::GetVertexBufferLayout()
	{
		if (m_HasUV)
		{
			return Nexus::Graphics::VertexBufferLayout(
				{{Nexus::Graphics::ShaderDataType::R32G32B32_SFloat, "TEXCOORD"}, {Nexus::Graphics::ShaderDataType::R32G32_SFloat, "TEXCOORD"}},
				sizeof(glm::vec3) + sizeof(glm::vec2),
				StepRate::Vertex);
		}
		else
		{
			return Nexus::Graphics::VertexBufferLayout({{Nexus::Graphics::ShaderDataType::R32G32B32_SFloat, "TEXCOORD"}},
													   sizeof(glm::vec3),
													   StepRate::Vertex);
		}
	}

	void FullscreenQuad::CreateWithUV()
	{
		float correction = m_GraphicsDevice->GetUVCorrection();

		std::vector<VertexPositionTexCoord> vertices = {{{-1.0f, -1.0f * correction, 0.0f}, {0.0f, 0.0f}},
														{{1.0f, -1.0f * correction, 0.0f}, {1.0f, 0.0f}},
														{{1.0f, 1.0f * correction, 0.0f}, {1.0f, 1.0f}},
														{{-1.0f, 1.0f * correction, 0.0f}, {0.0f, 1.0f}}};

		std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};

		m_VertexBuffer = Utils::CreateFilledVertexBuffer(vertices.data(),
														 vertices.size() * sizeof(VertexPositionTexCoord),
														 sizeof(VertexPositionTexCoord),
														 m_GraphicsDevice,
														 m_CommandQueue);

		m_IndexBuffer =
			Utils::CreateFilledIndexBuffer(indices.data(), indices.size() * sizeof(uint32_t), sizeof(uint32_t), m_GraphicsDevice, m_CommandQueue);
	}

	void FullscreenQuad::CreateWithoutUV()
	{
		float correction = m_GraphicsDevice->GetUVCorrection();

		std::vector<VertexPosition> vertices = {{{-1.0f, -1.0f * correction, 0.0f}},
												{{1.0f, -1.0f * correction, 0.0f}},
												{{1.0f, 1.0f * correction, 0.0f}},
												{{-1.0f, 1.0f * correction, 0.0f}}};

		std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};

		m_VertexBuffer = Utils::CreateFilledVertexBuffer(vertices.data(),
														 vertices.size() * sizeof(VertexPosition),
														 sizeof(VertexPosition),
														 m_GraphicsDevice,
														 m_CommandQueue);

		m_IndexBuffer =
			Utils::CreateFilledIndexBuffer(indices.data(), indices.size() * sizeof(uint32_t), sizeof(uint32_t), m_GraphicsDevice, m_CommandQueue);
	}

}	 // namespace Nexus::Graphics