#pragma once

#include "RHI/CommandQueue.hpp"
#include "RHI/GraphicsDevice.hpp"

namespace Nexus::Graphics
{
	class FullscreenQuad
	{
	  public:
		FullscreenQuad() = default;
		explicit FullscreenQuad(IGraphicsDevice *device, CommandQueueHandle commandQueue, bool hasUv);

		Ref<IDeviceBuffer> GetVertexBuffer();
		Ref<IDeviceBuffer> GetIndexBuffer();

		Nexus::Graphics::VertexBufferLayout GetVertexBufferLayout();

	  private:
		void CreateWithUV();
		void CreateWithoutUV();

	  private:
		IGraphicsDevice	  *m_GraphicsDevice = nullptr;
		CommandQueueHandle m_CommandQueue	= {};
		Ref<IDeviceBuffer> m_VertexBuffer	= nullptr;
		Ref<IDeviceBuffer> m_IndexBuffer	= nullptr;
		bool			   m_HasUV			= false;
	};
}	 // namespace Nexus::Graphics