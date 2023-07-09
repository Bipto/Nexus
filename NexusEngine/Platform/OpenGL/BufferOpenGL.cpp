#include "BufferOpenGL.h"
#include "Core/Logging/Log.h"

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

    DeviceBufferOpenGL::DeviceBufferOpenGL(const BufferDescription &description, const void *data)
        : DeviceBuffer(description, data)
    {
        GLenum bufferType = GetBufferType(m_Description.Type);
        GLenum bufferUsage = GetBufferUsage(m_Description.Usage);

        GL::ClearErrors();

        glGenBuffers(1, &m_Buffer);
        glBindBuffer(bufferType, m_Buffer);
        glBufferData(bufferType, description.Size, data, bufferUsage);

        GL::CheckErrors();
    }

    void DeviceBufferOpenGL::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        if (m_Description.Usage == BufferUsage::Static)
        {
            throw std::runtime_error("Attempting to upload data to a static buffer");
            return;
        }

        GLenum bufferType = GetBufferType(m_Description.Type);

        glBindBuffer(bufferType, m_Buffer);
        glBufferSubData(bufferType, offset, size, data);
    }

    void DeviceBufferOpenGL::Bind()
    {
        GLenum bufferType = GetBufferType(m_Description.Type);
        glBindBuffer(bufferType, m_Buffer);
    }

    unsigned int DeviceBufferOpenGL::GetHandle()
    {
        return m_Buffer;
    }
}