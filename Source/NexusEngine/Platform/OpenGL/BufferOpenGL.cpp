#if defined(NX_PLATFORM_OPENGL)

#include "BufferOpenGL.hpp"
#include "Nexus/Logging/Log.hpp"

namespace Nexus::Graphics
{
    void GetBaseType(const VertexBufferElement element, GLenum &baseType, uint32_t &componentCount, GLboolean &normalized)
    {
        switch (element.Type)
        {
        case ShaderDataType::Byte:
            baseType = GL_BYTE;
            componentCount = 1;
            normalized = false;
            break;
        case ShaderDataType::Byte2:
            baseType = GL_BYTE;
            componentCount = 2;
            normalized = false;
            break;
        case ShaderDataType::Byte4:
            baseType = GL_BYTE;
            componentCount = 4;
            normalized = false;
            break;

        case ShaderDataType::NormByte:
            baseType = GL_UNSIGNED_BYTE;
            componentCount = 1;
            normalized = true;
            break;
        case ShaderDataType::NormByte2:
            baseType = GL_UNSIGNED_BYTE;
            componentCount = 2;
            normalized = true;
            break;

        case ShaderDataType::NormByte4:
            baseType = GL_UNSIGNED_BYTE;
            componentCount = 4;
            normalized = true;
            break;

        case ShaderDataType::Float:
            baseType = GL_FLOAT;
            componentCount = 1;
            normalized = false;
            break;
        case ShaderDataType::Float2:
            baseType = GL_FLOAT;
            componentCount = 2;
            normalized = false;
            break;
        case ShaderDataType::Float3:
            baseType = GL_FLOAT;
            componentCount = 3;
            normalized = false;
            break;
        case ShaderDataType::Float4:
            baseType = GL_FLOAT;
            componentCount = 4;
            normalized = false;
            break;

        case ShaderDataType::Half:
            baseType = GL_HALF_FLOAT;
            componentCount = 1;
            normalized = true;
            break;
        case ShaderDataType::Half2:
            baseType = GL_HALF_FLOAT;
            componentCount = 2;
            normalized = true;
            break;
        case ShaderDataType::Half4:
            baseType = GL_HALF_FLOAT;
            componentCount = 4;
            normalized = true;
            break;

        case ShaderDataType::Int:
            baseType = GL_INT;
            componentCount = 1;
            normalized = false;
            break;
        case ShaderDataType::Int2:
            baseType = GL_INT;
            componentCount = 2;
            normalized = false;
            break;
        case ShaderDataType::Int3:
            baseType = GL_INT;
            componentCount = 3;
            normalized = false;
            break;
        case ShaderDataType::Int4:
            baseType = GL_INT;
            componentCount = 4;
            normalized = false;
            break;

        case ShaderDataType::SignedByte:
            baseType = GL_BYTE;
            componentCount = 1;
            normalized = false;
            break;
        case ShaderDataType::SignedByte2:
            baseType = GL_BYTE;
            componentCount = 2;
            normalized = false;
            break;
        case ShaderDataType::SignedByte4:
            baseType = GL_BYTE;
            componentCount = 4;
            normalized = false;
            break;

        case ShaderDataType::SignedByteNormalized:
            baseType = GL_BYTE;
            componentCount = 1;
            normalized = true;
            break;
        case ShaderDataType::SignedByte2Normalized:
            baseType = GL_BYTE;
            componentCount = 2;
            normalized = true;
            break;
        case ShaderDataType::SignedByte4Normalized:
            baseType = GL_BYTE;
            componentCount = 4;
            normalized = true;
            break;

        case ShaderDataType::Short:
            baseType = GL_SHORT;
            componentCount = 1;
            normalized = false;
            break;
        case ShaderDataType::Short2:
            baseType = GL_SHORT;
            componentCount = 2;
            normalized = false;
            break;
        case ShaderDataType::Short4:
            baseType = GL_SHORT;
            componentCount = 4;
            normalized = false;
            break;

        case ShaderDataType::ShortNormalized:
            baseType = GL_SHORT;
            componentCount = 1;
            normalized = true;
            break;
        case ShaderDataType::Short2Normalized:
            baseType = GL_SHORT;
            componentCount = 2;
            normalized = true;
            break;
        case ShaderDataType::Short4Normalized:
            baseType = GL_SHORT;
            componentCount = 4;
            normalized = true;
            break;

        case ShaderDataType::UInt:
            baseType = GL_UNSIGNED_INT;
            componentCount = 1;
            normalized = false;
            break;
        case ShaderDataType::UInt2:
            baseType = GL_UNSIGNED_INT;
            componentCount = 2;
            normalized = false;
            break;
        case ShaderDataType::UInt3:
            baseType = GL_UNSIGNED_INT;
            componentCount = 3;
            normalized = false;
            break;
        case ShaderDataType::UInt4:
            baseType = GL_UNSIGNED_INT;
            componentCount = 4;
            normalized = false;
            break;

        case ShaderDataType::UShort:
            baseType = GL_UNSIGNED_SHORT;
            componentCount = 1;
            normalized = false;
            break;
        case ShaderDataType::UShort2:
            baseType = GL_UNSIGNED_SHORT;
            componentCount = 2;
            normalized = false;
            break;
        case ShaderDataType::UShort4:
            baseType = GL_UNSIGNED_SHORT;
            componentCount = 4;
            normalized = false;
            break;

        case ShaderDataType::UShortNormalized:
            baseType = GL_UNSIGNED_SHORT;
            componentCount = 1;
            normalized = true;
            break;
        case ShaderDataType::UShort2Normalized:
            baseType = GL_UNSIGNED_SHORT;
            componentCount = 2;
            normalized = true;
            break;
        case ShaderDataType::UShort4Normalized:
            baseType = GL_UNSIGNED_SHORT;
            componentCount = 4;
            normalized = true;
            break;
        default:
            throw std::runtime_error("Failed to find valid vertex buffer element type");
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

        glGenVertexArrays(1, &m_VAO);
        SetupVertexArray();
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

    void VertexBufferOpenGL::SetupVertexArray(uint32_t offset)
    {
        offset *= m_Layout.GetStride();

        glBindVertexArray(m_VAO);
        int index = 0;
        for (auto &element : m_Layout)
        {
            GLenum baseType;
            uint32_t componentCount;
            GLboolean normalized;
            GetBaseType(element, baseType, componentCount, normalized);

            glVertexAttribPointer(
                index,
                componentCount,
                baseType,
                normalized,
                m_Layout.GetStride(),
                (void *)(element.Offset + offset));

            glEnableVertexAttribArray(index);
            index++;
        }
    }

    void VertexBufferOpenGL::UnbindVertexArray()
    {
        glBindVertexArray(0);
    }

    void *VertexBufferOpenGL::Map()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
        return glMapBufferRange(GL_ARRAY_BUFFER, 0, m_Description.Size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
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
        return glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, m_Description.Size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
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

    void UniformBufferOpenGL::Bind(uint32_t binding)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer);
        glBindBufferRange(GL_UNIFORM_BUFFER, binding, m_Buffer, 0, m_Description.Size);
    }

    unsigned int UniformBufferOpenGL::GetHandle()
    {
        return m_Buffer;
    }

    void *UniformBufferOpenGL::Map()
    {
        glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer);
        return glMapBufferRange(GL_UNIFORM_BUFFER, 0, m_Description.Size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    }

    void UniformBufferOpenGL::Unmap()
    {
        glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer);
        glUnmapBuffer(GL_UNIFORM_BUFFER);
    }
}

#endif