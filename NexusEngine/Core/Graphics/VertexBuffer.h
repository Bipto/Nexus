#pragma once

namespace NexusEngine
{
    class VertexBuffer
    {
        public:
            VertexBuffer(float vertices[], unsigned int size, unsigned int vertexStride, unsigned int vertexOffset, unsigned int vertexCount)
            {
                this->m_VertexStride = vertexStride;
                this->m_VertexOffset = vertexOffset;
                this->m_VertexCount = vertexCount;
            }
            VertexBuffer(const VertexBuffer&) = delete;
            virtual void Bind() = 0;
         private:
            unsigned int m_VertexStride;
            unsigned int m_VertexOffset;
            unsigned int m_VertexCount;
    };
}