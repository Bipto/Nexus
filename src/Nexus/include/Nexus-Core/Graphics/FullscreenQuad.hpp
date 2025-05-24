#pragma once

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

namespace Nexus::Graphics
{
	class FullscreenQuad
	{
	  public:
		FullscreenQuad() = default;
		explicit FullscreenQuad(GraphicsDevice *device, bool hasUv);

		Ref<DeviceBuffer> GetVertexBuffer();
		Ref<DeviceBuffer> GetIndexBuffer();

		Nexus::Graphics::VertexBufferLayout GetVertexBufferLayout();

	   private:
		 void CreateWithUV();
		 void CreateWithoutUV();

	   private:
		 GraphicsDevice	  *m_GraphicsDevice = nullptr;
		 Ref<DeviceBuffer> m_VertexBuffer	= nullptr;
		 Ref<DeviceBuffer> m_IndexBuffer	= nullptr;
		 bool			   m_HasUV			= false;
	};
}	 // namespace Nexus::Graphics