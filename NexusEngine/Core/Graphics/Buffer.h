#pragma once

#include "Core/nxpch.h"
#include "Core/Vertex.h"

namespace Nexus
{
    class VertexBuffer
    {
        public:
            VertexBuffer(const std::vector<float>& vertices){}
            VertexBuffer(const VertexBuffer&) = delete;
            virtual unsigned int GetVertexCount() = 0;
    };

    class IndexBuffer
    {
        public:
            IndexBuffer(const std::vector<unsigned int>& indices){}
            IndexBuffer(const IndexBuffer&) = delete;
            virtual unsigned int GetIndexCount() = 0;
    };
}