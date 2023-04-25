#pragma once

#include "Core/nxpch.h"
#include "Core/Vertex.h"

namespace Nexus
{
    class VertexBuffer
    {
        public:
            VertexBuffer(const std::vector<float>& vertices){}
            virtual ~VertexBuffer(){}
            virtual unsigned int GetVertexCount() = 0;
    };

    class IndexBuffer
    {
        public:
            IndexBuffer(const std::vector<unsigned int>& indices){}
            virtual ~IndexBuffer(){}
            virtual unsigned int GetIndexCount() = 0;
    };

    class UniformBuffer
    {
        public:
            UniformBuffer(uint32_t size, uint32_t binding){}
            virtual ~UniformBuffer(){}
            virtual void SetData(const void* data, uint32_t size, uint32_t offset) = 0;
    };
}