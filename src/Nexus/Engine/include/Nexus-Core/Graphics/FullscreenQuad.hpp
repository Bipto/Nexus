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

        DeviceBufferHandle GetVertexBuffer();
        DeviceBufferHandle GetIndexBuffer();

        Nexus::Graphics::VertexBufferLayout GetVertexBufferLayout();

      private:
        void CreateWithUV();
        void CreateWithoutUV();

      private:
        IGraphicsDevice *m_GraphicsDevice = nullptr;
        CommandQueueHandle m_CommandQueue = {};
        DeviceBufferHandle m_VertexBuffer = {};
        DeviceBufferHandle m_IndexBuffer = {};
        bool m_HasUV = false;
    };
} // namespace Nexus::Graphics