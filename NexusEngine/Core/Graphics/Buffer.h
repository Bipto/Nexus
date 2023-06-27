#pragma once

#include "Core/nxpch.h"
#include "Core/Vertex.h"

#include "Shader.h"

namespace Nexus
{
    /// @brief A pure virtual class that represents an API specific vertex buffer
    class VertexBuffer
    {
    public:
        /// @brief A constructor that creates a vertex buffer and takes in a vector of vertices
        /// @param vertices The vertices to create the vertex buffer with
        VertexBuffer(const std::vector<Vertex> &vertices) {}

        /// @brief A virtual destructor, allowing resources to be deleted
        virtual ~VertexBuffer() {}

        /// @brief A method that returns the number of vertices in the vertex buffer
        /// @return The number of vertices in the vertex buffer
        virtual unsigned int GetVertexCount() = 0;
    };

    /// @brief A pure virtual class that represents an API specific index buffer
    class IndexBuffer
    {
    public:
        /// @brief A constructor that creates an index buffer and takes in a vector of indices
        /// @param indices The indices to create the index buffer with
        IndexBuffer(const std::vector<unsigned int> &indices) {}

        /// @brief A virtual destructor, allowing resources to be deleted
        virtual ~IndexBuffer() {}

        /// @brief A method that returns the number of indices stored in the index buffer
        /// @return The number of indices in the index buffer
        virtual unsigned int GetIndexCount() = 0;
    };

    /// @brief A class representing a binding to a uniform buffer
    struct UniformResourceBinding
    {
        /// @brief The total size of the uniform buffer in bytes
        uint32_t Size;

        /// @brief The binding index of the uniform buffer
        uint32_t Binding;

        /// @brief The name of the uniform buffer
        std::string Name;
    };

    /// @brief A pure virtual class representing an API specific uniform buffer
    class UniformBuffer
    {
    public:
        /// @brief A constructor that creates a uniform buffer from a specified binding
        /// @param binding The binding to create the uniform buffer with
        UniformBuffer(const UniformResourceBinding &binding) {}

        /// @brief A virtual destructor, allowing resources to be deleted
        virtual ~UniformBuffer() {}

        /// @brief A method that sets the data of the uniform buffer
        /// @param data A pointer to the data to be uploaded to the uniform buffer
        /// @param size The size of the data to be uploaded to the uniform buffer
        /// @param offset An offset for the data being uploaded
        virtual void SetData(const void *data, uint32_t size, uint32_t offset) = 0;

        /// @brief A virtual method that binds a uniform buffer to a shader
        /// @param shader The shader to bind to the shader
        virtual void BindToShader(Ref<Shader> shader) {}
    };
}