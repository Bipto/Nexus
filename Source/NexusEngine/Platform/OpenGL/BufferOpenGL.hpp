#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "GL.hpp"
#include "Nexus/Graphics/Buffer.hpp"

namespace Nexus::Graphics
{
    class VertexBufferOpenGL : public VertexBuffer
    {
    public:
        VertexBufferOpenGL(const BufferDescription &description, const void *data);
        ~VertexBufferOpenGL();
        void Bind();
        unsigned int GetHandle();

        virtual void SetData(const void *data, uint32_t size, uint32_t offset = 0) override;

    private:
        unsigned int m_Buffer = 0;
    };

    class IndexBufferOpenGL : public IndexBuffer
    {
    public:
        IndexBufferOpenGL(const BufferDescription &description, const void *data, IndexBufferFormat format);
        virtual ~IndexBufferOpenGL();
        void Bind();
        unsigned int GetHandle();

        virtual void SetData(const void *data, uint32_t size, uint32_t offset = 0) override;

    private:
        unsigned int m_Buffer = 0;
    };

    class UniformBufferOpenGL : public UniformBuffer
    {
    public:
        UniformBufferOpenGL(const BufferDescription &description, const void *data);
        virtual ~UniformBufferOpenGL();
        unsigned int GetHandle();

        virtual void SetData(const void *data, uint32_t size, uint32_t offset = 0) override;

    private:
        unsigned int m_Buffer = 0;
    };
}

#endif