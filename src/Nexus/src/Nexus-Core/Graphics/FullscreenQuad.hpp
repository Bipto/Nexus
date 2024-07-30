#pragma once

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

namespace Nexus::Graphics
{
    class FullscreenQuad
    {
    public:
        FullscreenQuad() = default;
        explicit FullscreenQuad(GraphicsDevice *device);

        Ref<VertexBuffer> GetVertexBuffer();
        Ref<IndexBuffer> GetIndexBuffer();

        Nexus::Graphics::VertexBufferLayout GetVertexBufferLayout();

    private:
        Ref<VertexBuffer> m_VertexBuffer = nullptr;
        Ref<IndexBuffer> m_IndexBuffer = nullptr;
    };
}