#include "BufferOpenGL.hpp"
#include "Core/Logging/Log.hpp"

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

    GLenum GetMapMode(MapMode mode)
    {
        switch (mode)
        {
        case MapMode::Read:
            return GL_READ_ONLY;
        case MapMode::Write:
            return GL_WRITE_ONLY;
        case MapMode::ReadWrite:
            return GL_READ_WRITE;
        default:
            throw std::runtime_error("Invalid map mode entered");
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
    }

    void *VertexBufferOpenGL::Map(MapMode mode)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
        return glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    }

    void VertexBufferOpenGL::Unmap()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
        glUnmapBuffer(GL_ARRAY_BUFFER);
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

    IndexBufferOpenGL::IndexBufferOpenGL(const BufferDescription &description, const void *data)
        : IndexBuffer(description, data)
    {
        GLenum bufferUsage = GetBufferUsage(m_Description.Usage);

        GL::ClearErrors();
        glGenBuffers(1, &m_Buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, description.Size, data, bufferUsage);
        GL::CheckErrors();
    }

    void *IndexBufferOpenGL::Map(MapMode mode)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
        return glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
    }

    void IndexBufferOpenGL::Unmap()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    }

    void IndexBufferOpenGL::Bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
    }

    unsigned int IndexBufferOpenGL::GetHandle()
    {
        return m_Buffer;
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

    void *UniformBufferOpenGL::Map(MapMode mode)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer);
        return glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    }

    void UniformBufferOpenGL::Unmap()
    {
        glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer);
        glUnmapBuffer(GL_UNIFORM_BUFFER);
    }

    unsigned int UniformBufferOpenGL::GetHandle()
    {
        return m_Buffer;
    }
}