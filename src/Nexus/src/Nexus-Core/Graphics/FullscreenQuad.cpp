#include "Nexus-Core/Graphics/FullscreenQuad.hpp"

namespace Nexus::Graphics
{
	FullscreenQuad::FullscreenQuad(GraphicsDevice *device)
	{
		float correction = device->GetUVCorrection();

		std::vector<VertexPositionTexCoord> vertices = {{{-1.0f, -1.0f * correction, 0.0f}, {0.0f, 0.0f}},
														{{1.0f, -1.0f * correction, 0.0f}, {1.0f, 0.0f}},
														{{1.0f, 1.0f * correction, 0.0f}, {1.0f, 1.0f}},
														{{-1.0f, 1.0f * correction, 0.0f}, {0.0f, 1.0f}}};

		std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};

		Nexus::Graphics::BufferDescription vertexBufferDesc;
		vertexBufferDesc.Size  = vertices.size() * sizeof(VertexPositionTexCoord);
		vertexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
		m_VertexBuffer		   = device->CreateVertexBuffer(vertexBufferDesc, vertices.data());

		Nexus::Graphics::BufferDescription indexBufferDesc;
		indexBufferDesc.Size  = indices.size() * sizeof(unsigned int);
		indexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
		m_IndexBuffer		  = device->CreateIndexBuffer(indexBufferDesc, indices.data());
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
		Nexus::Graphics::VertexBufferLayout layout = {{Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"},
													  {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"}};
		return layout;
	}
}	 // namespace Nexus::Graphics