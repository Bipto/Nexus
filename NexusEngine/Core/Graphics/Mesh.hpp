#pragma once

#include "Core/Graphics/Buffer.hpp"
#include "Core/Memory.hpp"

#include "Core/Vertex.hpp"

namespace Nexus::Graphics
{
    /// @brief A class representing a mesh containing a vertex buffer and an index buffer
    class Mesh
    {
    public:
        /// @brief A default constructor creating a mesh with no geometry
        Mesh() = default;

        /// @brief A constructor taking in a vertex buffer and index buffer
        /// @param vertexBuffer A set of vertices to use for the mesh
        /// @param indexBuffer A set of indices to use for the mesh
        Mesh(Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer)
            : m_VertexBuffer(vertexBuffer), m_IndexBuffer(indexBuffer) {}

        /// @brief A method that returns a const reference to the meshes vertex buffer
        /// @return A const reference to the vertex buffer
        const Ref<VertexBuffer> &GetVertexBuffer() { return m_VertexBuffer; }

        /// @brief A method that returns a const reference to the meshes index buffer
        /// @return A const reference to the index buffer
        const Ref<IndexBuffer> &GetIndexBuffer() { return m_IndexBuffer; }

    private:
        /// @brief A reference counted pointer to a vertex buffer
        Ref<VertexBuffer> m_VertexBuffer = nullptr;

        /// @brief A reference counted pointer to an index buffer
        Ref<IndexBuffer> m_IndexBuffer = nullptr;
    };
}
