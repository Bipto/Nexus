#pragma once

#include "GL.hpp"
#include "Core/Graphics/Buffer.hpp"

#include "Platform/OpenGL/ShaderOpenGL.hpp"

namespace Nexus::Graphics
{
    class VertexBufferOpenGL : public VertexBuffer
    {
    public:
        VertexBufferOpenGL(const BufferDescription &description, const void *data, const VertexBufferLayout &layout);
        virtual void *Map(MapMode mode) override;
        virtual void Unmap() override;
        void Bind();
        unsigned int GetHandle();

    private:
        unsigned int m_Buffer = 0;
        unsigned int m_VAO = 0;
    };

    class IndexBufferOpenGL : public IndexBuffer
    {
    public:
        IndexBufferOpenGL(const BufferDescription &description, const void *data);
        virtual void *Map(MapMode mode) override;
        virtual void Unmap() override;
        void Bind();
        unsigned int GetHandle();

    private:
        unsigned int m_Buffer = 0;
    };

    class UniformBufferOpenGL : public UniformBuffer
    {
    public:
        UniformBufferOpenGL(const BufferDescription &description, const void *data);
        virtual void *Map(MapMode mode) override;
        virtual void Unmap() override;
        unsigned int GetHandle();

    private:
        unsigned int m_Buffer = 0;
    };
}