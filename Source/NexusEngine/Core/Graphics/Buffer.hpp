#pragma once

#include "Core/nxpch.hpp"
#include "Core/Vertex.hpp"

namespace Nexus::Graphics
{
    /// @brief An enum representing the different types of buffers that can be created
    enum class BufferType
    {
        /// @brief A default value used internally by the engine, should not be used by an application
        Invalid = 0,

        /// @brief A value representing a vertex buffer
        Vertex,

        /// @brief A value representing an index buffer
        Index,

        /// @brief A value representing a uniform buffer
        Uniform
    };

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

        /// @brief An enum value representing the type of buffer to create
        BufferType Type = BufferType::Invalid;

        /// @brief An enum value representing how the buffer can be accessed
        BufferUsage Usage = BufferUsage::Invalid;
    };

    /// @brief A class representing an API specific vertex buffer
    class VertexBuffer
    {
    public:
        /// @brief A constructor to create a new vertex buffer
        /// @param description A struct representing the properties to use when creating the buffer
        /// @param data A pointer to initial data to upload to the buffer
        /// @param layout A const reference to a vertex buffer layout, representing the contents of the vertex buffer
        VertexBuffer(const BufferDescription &description, const void *data, const VertexBufferLayout &layout)
            : m_Description(description), m_Layout(layout) {}

        /// @brief A pure virtual method to upload data to the GPU
        /// @param data A pointer to the data to upload
        /// @param size The total size in bytes of the data to upload
        /// @param offset An offset into the buffer to upload data to
        virtual void SetData(const void *data, uint32_t size, uint32_t offset) = 0;

        /// @brief A method that returns the buffer description that was used to create the buffer
        /// @return A const reference to the BufferDescription
        const BufferDescription &GetDescription() { return m_Description; }

        /// @brief A method that returns the layout of the contents of the buffer
        /// @return A const reference to the VertexBufferLayout
        const VertexBufferLayout &GetLayout() { return m_Layout; }

    protected:
        /// @brief A struct containing the properties that were used when creating the buffer
        BufferDescription m_Description;

        /// @brief A struct containing the layout of the vertex buffer
        VertexBufferLayout m_Layout;
    };

    /// @brief A class representing an API specific index buffer
    class IndexBuffer
    {
    public:
        /// @brief A constructor to create a new index buffer
        /// @param description A struct representing the properties to use when creating the buffer
        /// @param data A pointer to initial data to upload to the buffer
        IndexBuffer(const BufferDescription &description, const void *data)
            : m_Description(description) {}

        /// @brief A pure virtual method to upload data to the GPU
        /// @param data A pointer to the data to upload
        /// @param size The total size in bytes of the data to upload
        /// @param offset An offset into the buffer to upload data to
        virtual void SetData(const void *data, uint32_t size, uint32_t offset) = 0;

        /// @brief A method that returns the buffer description that was used to create the buffer
        /// @return A const reference to the BufferDescription
        const BufferDescription &GetDescription() { return m_Description; }

    protected:
        /// @brief A struct containing the properties that were used when creating the buffer
        BufferDescription m_Description;
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

        /// @brief A pure virtual method to upload data to the GPU
        /// @param data A pointer to the data to upload
        /// @param size The total size in bytes of the data to upload
        /// @param offset An offset into the buffer to upload data to
        virtual void SetData(const void *data, uint32_t size, uint32_t offset) = 0;

        /// @brief A method that returns the buffer description that was used to create the buffer
        /// @return A const reference to the BufferDescription
        const BufferDescription &GetDescription() { return m_Description; }

    protected:
        /// @brief A struct containing the properties that were used when creating the buffer
        BufferDescription m_Description;
    };
}