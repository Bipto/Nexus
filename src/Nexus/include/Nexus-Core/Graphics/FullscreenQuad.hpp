#pragma once

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

namespace Nexus::Graphics
{
	class FullscreenQuad
	{
	  public:
		NX_API FullscreenQuad() = default;
		NX_API explicit FullscreenQuad(GraphicsDevice *device, bool hasUv);

		NX_API Ref<VertexBuffer> GetVertexBuffer();
		NX_API Ref<IndexBuffer> GetIndexBuffer();

		NX_API Nexus::Graphics::VertexBufferLayout GetVertexBufferLayout();

	  private:
		void CreateWithUV();
		void CreateWithoutUV();

	  private:
		GraphicsDevice	 *m_GraphicsDevice = nullptr;
		Ref<VertexBuffer> m_VertexBuffer   = nullptr;
		Ref<IndexBuffer>  m_IndexBuffer	   = nullptr;
		bool			  m_HasUV		   = false;
	};
}	 // namespace Nexus::Graphics