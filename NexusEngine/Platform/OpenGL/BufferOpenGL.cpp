#include "BufferOpenGL.h"

#include "Core/Logging/Log.h"

namespace Nexus
{
    VertexBufferOpenGL::VertexBufferOpenGL(const std::vector<float>& vertices) 
        : VertexBuffer(vertices)
    {
        GL::ClearErrors();

        glGenBuffers(1, &this->m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        if (!GL::CheckErrors())
            NX_LOG("Vertex buffer created successfully");

        m_VertexCount = vertices.size();
    }

    void VertexBufferOpenGL::Bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);    
    }

    IndexBufferOpenGL::IndexBufferOpenGL(const std::vector<unsigned int>& indices)
        : IndexBuffer(indices)
    {
        GL::ClearErrors();

        glGenBuffers(1, &this->m_IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_IBO);
        size_t size = indices.size() * sizeof(unsigned int);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices.data(), GL_STATIC_DRAW);
    
        if (!GL::CheckErrors())
            NX_LOG("Index buffer created successfully");

        m_IndexCount = indices.size();
    }

    void IndexBufferOpenGL::Bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_IBO);
    }

    UniformBufferOpenGL::UniformBufferOpenGL(uint32_t size, uint32_t binding)
        : UniformBuffer(size, binding)
    {
        GL::ClearErrors();

        glCreateBuffers(1, &m_UBO);
        glNamedBufferData(m_UBO, size, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_UBO);

        if (!GL::CheckErrors())
            NX_LOG("Uniform buffer created successfully");
    }

    UniformBufferOpenGL::~UniformBufferOpenGL()
    {
        GL::ClearErrors();
        glDeleteBuffers(1, &m_UBO);
        GL::CheckErrors();
    }

    void UniformBufferOpenGL::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        GL::ClearErrors();
        glNamedBufferSubData(m_UBO, offset, size, data);
        if (!GL::CheckErrors())
            NX_LOG("Uniform buffer data uploaded successfully");
    }
}