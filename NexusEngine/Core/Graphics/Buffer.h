#pragma once

#include <vector>
#include "glm.hpp"

namespace NexusEngine
{
    class VertexBuffer
    {
        public:
            VertexBuffer(const std::vector<glm::vec3>& vertices){}
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