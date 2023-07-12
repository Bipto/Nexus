#pragma once
#include "glm/glm.hpp"

#include "Core/Graphics/ShaderDataType.hpp"

namespace Nexus::Graphics
{
    /// @brief A struct that represents an item within a vertex buffer
    struct VertexBufferElement
    {
        /// @brief The name of the element, needs to match the name in the shader
        std::string Name;

        /// @brief The type of the element
        ShaderDataType Type;

        /// @brief The total size of the element
        uint32_t Size = 0;

        /// @brief The offset of the element within each separate buffer item
        size_t Offset = 0;

        /// @brief Whether the data within the vertex buffer is normalized
        bool Normalized;

        /// @brief A default constructor to create an empty element
        VertexBufferElement() = default;

        /// @brief A constructor creating a description of a vertex buffer element
        /// @param type The type of the data within the buffer
        /// @param name The name of the item within the buffer
        /// @param normalized Whether the data is normalized
        VertexBufferElement(ShaderDataType type, const std::string &name, bool normalized = false);

        /// @brief A method that returns an unsigned 32 bit integer representing the number of components within the element
        /// @return The number of components within the element (e.g. Float2 will return 2)
        uint32_t GetComponentCount() const;
    };

    /// @brief A struct representing a set of vertex elements stored within a vertex buffer
    struct VertexBufferLayout
    {
    public:
        /// @brief A default constructor creating an empty vertex buffer layout
        VertexBufferLayout() = default;

        /// @brief A constructor taking in an initializer list of vertex buffer elements to use to create the layout
        /// @param elements An initializer list of vertex buffer elements
        VertexBufferLayout(std::initializer_list<VertexBufferElement> elements)
            : m_Elements(elements)
        {
            CalculateOffsetsAndStride();
        }

        /// @brief An iterator returning the beginning of the layout
        /// @return An iterator returning the first element of the layout
        std::vector<VertexBufferElement>::iterator begin() { return m_Elements.begin(); }

        /// @brief An iterator returning the end of the layout
        /// @return An iterator returning the last element of the layout
        std::vector<VertexBufferElement>::iterator end() { return m_Elements.end(); }

        /// @brief An iterator returning the beginning of the layout
        /// @return An iterator returning the first element of the layout
        std::vector<VertexBufferElement>::const_iterator begin() const { return m_Elements.begin(); }

        /// @brief An iterator returning the end of the layout
        /// @return An iterator returning the last element of the layout
        std::vector<VertexBufferElement>::const_iterator end() const { return m_Elements.end(); }

        /// @brief A method that returns the stride between elements in the vertex buffer
        /// @return An unsigned 32 bit integer representing the gap between separate items of the vertex buffer
        uint32_t GetStride() { return m_Stride; }

        /// @brief A method that returns the number of elements stored within the layout
        /// @return An unsigned 32 bit integer representing the number of elements within the layout
        uint32_t GetNumberOfElements() { return m_Elements.size(); }

    private:
        /// @brief A private method that calculates the offset of each element within the buffer and the stride (total distance between elements)
        void CalculateOffsetsAndStride();

    private:
        /// @brief A vector containing the elements within the vertex buffer
        std::vector<VertexBufferElement> m_Elements;

        /// @brief An unsigned 32 bit integer representing the stride between each separate vertex buffer item
        uint32_t m_Stride = 0;
    };

    /// @brief A struct representing a vertex with 3D position in world space
    struct VertexPosition
    {
        /// @brief 3 floating point values representing position of the vertex
        glm::vec3 Position = {0, 0, 0};

        /// @brief A constructor taking in the position of the vertex
        /// @param position A const reference to 3 floating point values representing the position
        VertexPosition(const glm::vec3 &position)
            : Position(position) {}

        /// @brief A static method that returns the vertex buffer layout of this vertex type
        /// @return A VertexBufferLayout object containing the buffer layout of the type
        static Nexus::Graphics::VertexBufferLayout GetLayout()
        {
            Nexus::Graphics::VertexBufferLayout layout =
                {
                    {Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"}};
            return layout;
        }
    };

    /// @brief A struct representing a vertex with 3D position in world space and a texture coordinate
    struct VertexPositionTexCoord
    {
        /// @brief 3 floating point values representing position of the vertex
        glm::vec3 Position = {0, 0, 0};

        /// @brief 2 floating point values representing the texture coordinates of the vertex
        glm::vec2 TexCoords = {0, 0};

        /// @brief A constructor taking in the position of the vertex and its texture coordinate
        /// @param position A const reference to 3 floating point values representing the position
        /// @param texCoords A const reference to 2 floating point values representing the texture coordinates
        VertexPositionTexCoord(const glm::vec3 &position, const glm::vec2 &texCoords)
            : Position(position), TexCoords(texCoords) {}

        /// @brief A static method that returns the vertex buffer layout of this vertex type
        /// @return A VertexBufferLayout object containing the buffer layout of the type
        static Nexus::Graphics::VertexBufferLayout GetLayout()
        {
            Nexus::Graphics::VertexBufferLayout layout =
                {
                    {Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"},
                    {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"}};
            return layout;
        }
    };

    /// @brief A struct representing a vertex with 3D position in world space, a texture coordinate and a normal
    struct VertexPositionTexCoordNormal
    {
        /// @brief 3 floating point values representing position of the vertex
        glm::vec3 Position = {0, 0, 0};

        /// @brief 2 floating point values representing the texture coordinates of the vertex
        glm::vec2 TexCoords = {0, 0};

        /// @brief 3 floating point values representing the normal of the vertex
        glm::vec3 Normal = {0, 0, 0};

        /// @brief A constructor taking in the position of the vertex, it's texture coordinate and it's normal
        /// @param position A const reference to 3 floating point values representing the position
        /// @param texCoords A const reference to 2 floating point values representing the texture coordinates
        /// @param normal A const reference to 3 floating point values representing the normal
        VertexPositionTexCoordNormal(const glm::vec3 &position, const glm::vec2 &texCoords, const glm::vec3 &normal)
            : Position(position), TexCoords(texCoords), Normal(normal) {}

        /// @brief A static method that returns the vertex buffer layout of this vertex type
        /// @return A VertexBufferLayout object containing the buffer layout of the type
        static Nexus::Graphics::VertexBufferLayout GetLayout()
        {
            Nexus::Graphics::VertexBufferLayout layout =
                {
                    {Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"},
                    {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"},
                    {Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"}};
            return layout;
        }
    };
}