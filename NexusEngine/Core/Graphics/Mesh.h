#pragma once

#include "Core/Graphics/Buffer.h"
#include "Core/Memory.h"

namespace Nexus
{
    class Mesh
    {
        public:
            Mesh() = default;
            Mesh(Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer)
                : m_VertexBuffer(vertexBuffer), m_IndexBuffer(indexBuffer){}

            const Ref<VertexBuffer>& GetVertexBuffer() { return m_VertexBuffer; }
            const Ref<IndexBuffer>& GetIndexBuffer() { return m_IndexBuffer; }

        private:
            Ref<VertexBuffer> m_VertexBuffer = nullptr;
            Ref<IndexBuffer> m_IndexBuffer   = nullptr;
    };
}
