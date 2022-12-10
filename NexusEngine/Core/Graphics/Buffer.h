#pragma once

#include <vector>
#include "Core/Vertex.h"

namespace Nexus
{
    class VertexBuffer
    {
        public:
            VertexBuffer(const std::vector<Vertex>& vertices){}
            VertexBuffer(const VertexBuffer&) = delete;
            virtual void Bind() = 0;
    };

    class IndexBuffer
    {
        public:
            IndexBuffer(unsigned int indices[], unsigned int size){}
            IndexBuffer(const IndexBuffer&) = delete;
            virtual void Bind() = 0;
    };
}