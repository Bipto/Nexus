#include "BufferOpenGL.hpp"
#include "Core/Logging/Log.hpp"

namespace Nexus::Graphics
{
    GLenum GetBufferType(BufferType type)
    {
        switch (type)
        {
        case BufferType::Vertex:
            return GL_ARRAY_BUFFER;
        case BufferType::Index:
            return GL_ELEMENT_ARRAY_BUFFER;
        case BufferType::Uniform:
            return GL_UNIFORM_BUFFER;
        default:
            throw std::runtime_error("Invalid buffer type entered");
        }
    }

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
    }

    void VertexBufferOpenGL::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        if (m_Description.Usage == BufferUsage::Static)
        {
            throw std::runtime_error("Attempting to upload data to a static buffer");
            return;
        }

        glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    }

    void VertexBufferOpenGL::Bind()
    {
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

    void IndexBufferOpenGL::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        if (m_Description.Usage == BufferUsage::Static)
        {
            throw std::runtime_error("Attempting to upload data to a static buffer");
            return;
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
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

    void UniformBufferOpenGL::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        if (m_Description.Usage == BufferUsage::Static)
        {
            throw std::runtime_error("Attempting to upload data to a static buffer");
            return;
        }

        glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    }
    unsigned int UniformBufferOpenGL::GetHandle()
    {
        return m_Buffer;
    }
}