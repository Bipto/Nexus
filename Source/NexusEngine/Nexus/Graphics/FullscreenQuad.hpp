#pragma once

#include "Nexus/Graphics/GraphicsDevice.hpp"

namespace Nexus::Graphics
{
    class FullscreenQuad
    {
    public:
        FullscreenQuad(GraphicsDevice *device);

        Ref<VertexBuffer> GetVertexBuffer();
        Ref<IndexBuffer> GetIndexBuffer();

        Nexus::Graphics::VertexBufferLayout GetVertexBufferLayout();

    private:
        Ref<VertexBuffer> m_VertexBuffer = nullptr;
        Ref<IndexBuffer> m_IndexBuffer = nullptr;
    };
}