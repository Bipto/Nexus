#include "Nexus-Core/Graphics/FullscreenQuad.hpp"

namespace Nexus::Graphics
{
	FullscreenQuad::FullscreenQuad(GraphicsDevice *device, bool hasUv) : m_GraphicsDevice(device), m_HasUV(hasUv)
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

	Ref<VertexBuffer> FullscreenQuad::GetVertexBuffer()
	{
		return m_VertexBuffer;
	}

	Ref<IndexBuffer> FullscreenQuad::GetIndexBuffer()
	{
		return m_IndexBuffer;
	}

	Nexus::Graphics::VertexBufferLayout FullscreenQuad::GetVertexBufferLayout()
	{
		if (m_HasUV)
		{
			return {{Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"}, {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"}};
		}
		else
		{
			return {{Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"}};
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

		Nexus::Graphics::BufferDescription vertexBufferDesc;
		vertexBufferDesc.Size  = vertices.size() * sizeof(VertexPositionTexCoord);
		vertexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
		m_VertexBuffer		   = m_GraphicsDevice->CreateVertexBuffer(vertexBufferDesc, vertices.data());

		Nexus::Graphics::BufferDescription indexBufferDesc;
		indexBufferDesc.Size  = indices.size() * sizeof(unsigned int);
		indexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
		m_IndexBuffer		  = m_GraphicsDevice->CreateIndexBuffer(indexBufferDesc, indices.data());
	}

	void FullscreenQuad::CreateWithoutUV()
	{
		float correction = m_GraphicsDevice->GetUVCorrection();

		std::vector<VertexPosition> vertices = {{{-1.0f, -1.0f * correction, 0.0f}},
												{{1.0f, -1.0f * correction, 0.0f}},
												{{1.0f, 1.0f * correction, 0.0f}},
												{{-1.0f, 1.0f * correction, 0.0f}}};

		std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};

		Nexus::Graphics::BufferDescription vertexBufferDesc;
		vertexBufferDesc.Size  = vertices.size() * sizeof(VertexPosition);
		vertexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
		m_VertexBuffer		   = m_GraphicsDevice->CreateVertexBuffer(vertexBufferDesc, vertices.data());

		Nexus::Graphics::BufferDescription indexBufferDesc;
		indexBufferDesc.Size  = indices.size() * sizeof(unsigned int);
		indexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
		m_IndexBuffer		  = m_GraphicsDevice->CreateIndexBuffer(indexBufferDesc, indices.data());
	}

}	 // namespace Nexus::Graphics