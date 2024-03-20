#pragma once

#include "Nexus/nxpch.hpp"
#include "Nexus/Vertex.hpp"

namespace Nexus::Graphics
{
    /// @brief An enum representing the different access modifiers for buffers
    enum class BufferUsage
    {
        /// @brief A default value used internally by the engine, should not be used by an application
        Invalid = 0,

        /// @brief A value representing a buffer that can only have it's data set upon initialisation
        Static,

        /// @brief A value representing a buffer that can be updated at any time
        Dynamic
    };

    /// @brief A struct representing a specification for a buffer
    struct BufferDescription
    {
        /// @brief The total size of the buffer in bytes
        uint32_t Size = 0;

        /// @brief An enum value representing how the buffer can be accessed
        BufferUsage Usage = BufferUsage::Invalid;
    };

    enum class IndexBufferFormat
    {
        UInt16,
        UInt32
    };

    /// @brief A class representing an API specific vertex buffer
    class VertexBuffer
    {
    public:
        /// @brief A constructor to create a new vertex buffer
        /// @param description A struct representing the properties to use when creating the buffer
        /// @param data A pointer to initial data to upload to the buffer
        /// @param layout A const reference to a vertex buffer layout, representing the contents of the vertex buffer
        VertexBuffer(const BufferDescription &description, const void *data)
            : m_Description(description) {}

        /// @brief A virtual destructor to cleanup resources
        virtual ~VertexBuffer() {}

        virtual void SetData(const void *data, uint32_t size, uint32_t offset = 0) = 0;

        /// @brief A method that returns the buffer description that was used to create the buffer
        /// @return A const reference to the BufferDescription
        const BufferDescription &GetDescription() { return m_Description; }

    protected:
        /// @brief A struct containing the properties that were used when creating the buffer
        BufferDescription m_Description;
    };

    /// @brief A class representing an API specific index buffer
    class IndexBuffer
    {
    public:
        /// @brief A constructor to create a new index buffer
        /// @param description A struct representing the properties to use when creating the buffer
        /// @param data A pointer to initial data to upload to the buffer
        IndexBuffer(const BufferDescription &description, const void *data, IndexBufferFormat format)
            : m_Description(description), m_Format(format) {}

        /// @brief A virtual destructor to cleanup resources
        virtual ~IndexBuffer() {}

        virtual void SetData(const void *data, uint32_t size, uint32_t offset = 0) = 0;

        /// @brief A method that returns the buffer description that was used to create the buffer
        /// @return A const reference to the BufferDescription
        const BufferDescription &GetDescription() { return m_Description; }

        IndexBufferFormat GetFormat() { return m_Format; }

    protected:
        /// @brief A struct containing the properties that were used when creating the buffer
        BufferDescription m_Description;

        /// @brief An enum indicating the format of the buffer
        IndexBufferFormat m_Format;
    };

    /// @brief A class representing an API specific uniform buffer
    class UniformBuffer
    {
    public:
        /// @brief A constructor to create a new uniform buffer
        /// @param description A struct representing the properties to use when creating the buffer
        /// @param data A pointer to initial data to upload to the buffer
        UniformBuffer(const BufferDescription &description, const void *data)
            : m_Description(description) {}

        /// @brief A virtual destructor to cleanup resources
        virtual ~UniformBuffer() {}

        virtual void SetData(const void *data, uint32_t size, uint32_t offset = 0) = 0;

        /// @brief A method that returns the buffer description that was used to create the buffer
        /// @return A const reference to the BufferDescription
        const BufferDescription &GetDescription() { return m_Description; }

    protected:
        /// @brief A struct containing the properties that were used when creating the buffer
        BufferDescription m_Description;
    };
}