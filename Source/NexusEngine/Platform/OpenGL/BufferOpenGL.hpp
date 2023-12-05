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
        void Bind();
        unsigned int GetHandle();

        virtual void *Map() override;
        virtual void Unmap() override;

    private:
        unsigned int m_Buffer = 0;
        unsigned int m_VAO = 0;
    };

    class IndexBufferOpenGL : public IndexBuffer
    {
    public:
        IndexBufferOpenGL(const BufferDescription &description, const void *data, IndexBufferFormat format);
        virtual ~IndexBufferOpenGL();
        void Bind();
        unsigned int GetHandle();

        virtual void *Map() override;
        virtual void Unmap() override;

    private:
        unsigned int m_Buffer = 0;
    };

    class UniformBufferOpenGL : public UniformBuffer
    {
    public:
        UniformBufferOpenGL(const BufferDescription &description, const void *data);
        virtual ~UniformBufferOpenGL();
        unsigned int GetHandle();

        virtual void *Map() override;
        virtual void Unmap() override;

    private:
        unsigned int m_Buffer = 0;
    };
}