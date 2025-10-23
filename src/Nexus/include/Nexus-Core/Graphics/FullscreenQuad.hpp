#pragma once

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

namespace Nexus::Graphics
{
	class FullscreenQuad
	{
	  public:
		FullscreenQuad() = default;
		explicit FullscreenQuad(IGraphicsDevice *device, Ref<ICommandQueue> commandQueue, bool hasUv);

		Ref<IDeviceBuffer> GetVertexBuffer();
		Ref<IDeviceBuffer> GetIndexBuffer();

		Nexus::Graphics::VertexBufferLayout GetVertexBufferLayout();

	  private:
		void CreateWithUV();
		void CreateWithoutUV();

	  private:
		IGraphicsDevice	  *m_GraphicsDevice = nullptr;
		Ref<ICommandQueue> m_CommandQueue	= nullptr;
		Ref<IDeviceBuffer>  m_VertexBuffer	= nullptr;
		Ref<IDeviceBuffer>  m_IndexBuffer	= nullptr;
		bool			   m_HasUV			= false;
	};
}	 // namespace Nexus::Graphics