#pragma once

#include "GL.h"
#include "Core/Graphics/Buffer.h"

#include "Platform/OpenGL/ShaderOpenGL.h"

namespace Nexus
{
    class VertexBufferOpenGL : public VertexBuffer
    {
    public:
        VertexBufferOpenGL(const std::vector<Vertex> &vertices);
        void Bind();
        virtual unsigned int GetVertexCount() override { return m_VertexCount; }

    private:
        unsigned int m_VBO = 0;
        unsigned int m_VertexCount = 0;
    };

    class IndexBufferOpenGL : public IndexBuffer
    {
    public:
        IndexBufferOpenGL(const std::vector<unsigned int> &indices);
        void Bind();
        virtual unsigned int GetIndexCount() { return m_IndexCount; }

    private:
        unsigned int m_IBO;
        unsigned int m_IndexCount;
    };

    class UniformBufferOpenGL : public UniformBuffer
    {
    public:
        UniformBufferOpenGL(const UniformResourceBinding &binding);
        virtual ~UniformBufferOpenGL();
        virtual void SetData(const void *data, uint32_t size, uint32_t offset) override;
        virtual void BindToShader(Ref<Shader> shader) override;

    private:
        unsigned int m_UBO = 0;
        UniformResourceBinding m_Binding;
    };
}