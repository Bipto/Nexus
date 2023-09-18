#pragma once

#include "GL.hpp"
#include "Nexus/Graphics/Buffer.hpp"

#include "Platform/OpenGL/ShaderOpenGL.hpp"

namespace Nexus::Graphics
{
    class VertexBufferOpenGL : public VertexBuffer
    {
    public:
        VertexBufferOpenGL(const BufferDescription &description, const void *data, const VertexBufferLayout &layout);
        ~VertexBufferOpenGL();
        virtual void SetData(const void *data, uint32_t size, uint32_t offset) override;
        void Bind();
        unsigned int GetHandle();

    private:
        unsigned int m_Buffer = 0;
        unsigned int m_VAO = 0;
    };

    class IndexBufferOpenGL : public IndexBuffer
    {
    public:
        IndexBufferOpenGL(const BufferDescription &description, const void *data, IndexBufferFormat format);
        virtual ~IndexBufferOpenGL();
        virtual void SetData(const void *data, uint32_t size, uint32_t offset) override;
        void Bind();
        unsigned int GetHandle();

    private:
        unsigned int m_Buffer = 0;
    };

    class UniformBufferOpenGL : public UniformBuffer
    {
    public:
        UniformBufferOpenGL(const BufferDescription &description, const void *data);
        virtual ~UniformBufferOpenGL();
        virtual void SetData(const void *data, uint32_t size, uint32_t offset) override;
        unsigned int GetHandle();

    private:
        unsigned int m_Buffer = 0;
    };
}