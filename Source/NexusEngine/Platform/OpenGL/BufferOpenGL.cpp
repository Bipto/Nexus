#include "BufferOpenGL.hpp"
#include "Nexus/Logging/Log.hpp"

namespace Nexus::Graphics
{
    GLenum GetBufferUsage(BufferUsage usage)
    {
        switch (usage)
        {
        case BufferUsage::Dynamic:
            return GL_DYNAMIC_DRAW;
        case BufferUsage::Static:
            return GL_STATIC_DRAW;
        default:
            throw std::runtime_error("Invalid buffer usage entered");
        }
    }

    VertexBufferOpenGL::VertexBufferOpenGL(const BufferDescription &description, const void *data, const VertexBufferLayout &layout)
        : VertexBuffer(description, data, layout)
    {
        GLenum bufferUsage = GetBufferUsage(m_Description.Usage);

        GL::ClearErrors();
        glGenBuffers(1, &m_Buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
        glBufferData(GL_ARRAY_BUFFER, description.Size, data, bufferUsage);
        GL::CheckErrors();

        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);
        int index = 0;
        for (auto &element : m_Layout)
        {
            glVertexAttribPointer(index,
                                  element.GetComponentCount(),
                                  GetGLBaseType(element),
                                  element.Normalized ? GL_TRUE : GL_FALSE,
                                  m_Layout.GetStride(),
                                  (void *)element.Offset);

            glEnableVertexAttribArray(index);
            index++;
        }
        glBindVertexArray(0);
    }

    VertexBufferOpenGL::~VertexBufferOpenGL()
    {
        glDeleteBuffers(1, &m_Buffer);
        glDeleteVertexArrays(1, &m_VAO);
    }

    void VertexBufferOpenGL::Bind()
    {
        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
    }

    unsigned int VertexBufferOpenGL::GetHandle()
    {
        return m_Buffer;
    }

    void *VertexBufferOpenGL::Map()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
        return glMapBufferRange(GL_ARRAY_BUFFER, 0, m_Description.Size, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
    }

    void VertexBufferOpenGL::Unmap()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }

    IndexBufferOpenGL::IndexBufferOpenGL(const BufferDescription &description, const void *data, IndexBufferFormat format)
        : IndexBuffer(description, data, format)
    {
        GLenum bufferUsage = GetBufferUsage(m_Description.Usage);

        GL::ClearErrors();
        glGenBuffers(1, &m_Buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, description.Size, data, bufferUsage);
        GL::CheckErrors();
    }

    IndexBufferOpenGL::~IndexBufferOpenGL()
    {
        glDeleteBuffers(1, &m_Buffer);
    }

    void IndexBufferOpenGL::Bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
    }

    unsigned int IndexBufferOpenGL::GetHandle()
    {
        return m_Buffer;
    }

    void *IndexBufferOpenGL::Map()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
        return glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, m_Description.Size, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
    }

    void IndexBufferOpenGL::Unmap()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    }

    UniformBufferOpenGL::UniformBufferOpenGL(const BufferDescription &description, const void *data)
        : UniformBuffer(description, data)
    {
        GLenum bufferUsage = GetBufferUsage(m_Description.Usage);

        GL::ClearErrors();
        glGenBuffers(1, &m_Buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer);
        glBufferData(GL_UNIFORM_BUFFER, description.Size, data, bufferUsage);
        GL::CheckErrors();
    }

    UniformBufferOpenGL::~UniformBufferOpenGL()
    {
        glDeleteBuffers(1, &m_Buffer);
    }

    unsigned int UniformBufferOpenGL::GetHandle()
    {
        return m_Buffer;
    }

    void *UniformBufferOpenGL::Map()
    {
        glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer);
        return glMapBufferRange(GL_UNIFORM_BUFFER, 0, m_Description.Size, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
    }

    void UniformBufferOpenGL::Unmap()
    {
        glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer);
        glUnmapBuffer(GL_UNIFORM_BUFFER);
    }
}