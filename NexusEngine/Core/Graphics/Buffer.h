#pragma once

#include "Core/nxpch.h"
#include "Core/Vertex.h"

#include "Shader.h"

namespace Nexus
{
    class VertexBuffer
    {
        public:
            VertexBuffer(const std::vector<Vertex>& vertices){}
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
    
    struct UniformResourceBinding
    {
        uint32_t Size;
        uint32_t Binding;
        std::string Name;
    };

    class UniformBuffer
    {
        public:
            UniformBuffer(const UniformResourceBinding& binding){}
            virtual ~UniformBuffer(){}
            virtual void SetData(const void* data, uint32_t size, uint32_t offset) = 0;
            virtual void BindToShader(Ref<Shader> shader) {}
    };
}