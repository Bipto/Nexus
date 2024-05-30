#if defined(NX_PLATFORM_OPENGL)

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

    VertexBufferOpenGL::VertexBufferOpenGL(const BufferDescription &description, const void *data)
        : VertexBuffer(description, data)
    {
        GLenum bufferUsage = GetBufferUsage(m_Description.Usage);

        GL::ClearErrors();
        glGenBuffers(1, &m_Buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
        glBufferData(GL_ARRAY_BUFFER, description.Size, data, bufferUsage);
        GL::CheckErrors();
    }

    VertexBufferOpenGL::~VertexBufferOpenGL()
    {
        GL::ClearErrors();
        glDeleteBuffers(1, &m_Buffer);
        GL::CheckErrors();
    }

    void VertexBufferOpenGL::Bind()
    {
        GL::ClearErrors();
        glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
        GL::CheckErrors();
    }

    void VertexBufferOpenGL::Unbind()
    {
        GL::ClearErrors();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        GL::CheckErrors();
    }

    unsigned int VertexBufferOpenGL::GetHandle()
    {
        return m_Buffer;
    }

    void VertexBufferOpenGL::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        GL::ClearErrors();
        glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
        GL::CheckErrors();
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
        GL::ClearErrors();
        glDeleteBuffers(1, &m_Buffer);
        GL::CheckErrors();
    }

    void IndexBufferOpenGL::Bind()
    {
        GL::ClearErrors();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
        GL::CheckErrors();
    }

    void IndexBufferOpenGL::Unbind()
    {
        GL::ClearErrors();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        GL::CheckErrors();
    }

    unsigned int IndexBufferOpenGL::GetHandle()
    {
        return m_Buffer;
    }

    void IndexBufferOpenGL::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        GL::ClearErrors();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
        GL::CheckErrors();
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
        GL::ClearErrors();
        glDeleteBuffers(1, &m_Buffer);
        GL::CheckErrors();
    }

    unsigned int UniformBufferOpenGL::GetHandle()
    {
        return m_Buffer;
    }

    void UniformBufferOpenGL::Unbind()
    {
        GL::ClearErrors();
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        GL::CheckErrors();
    }

    void UniformBufferOpenGL::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        GL::ClearErrors();
        glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
        GL::CheckErrors();
    }
}

#endif