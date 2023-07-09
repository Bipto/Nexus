#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "ShaderDataType.h"
#include "Texture.h"
#include "Core/Memory.h"
#include "Buffer.h"

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

    /// @brief A struct representing a binding of a texture within a shader
    struct TextureBinding
    {
        /// @brief An integer representing which slot the texture should be bound to
        int Slot;

        /// @brief The name of the texture (must match the name within the shader as this is required by some APIs)
        std::string Name;
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

    /// @brief A pure virtual class representing an API specific shader
    class Shader
    {
    public:
        /// @brief A pure virtual method that binds a texture to a shader
        /// @param texture A reference counted pointer to the texture to bind
        /// @param binding A const reference to a TextureBinding mapping where the texture should be bound
        virtual void SetTexture(Ref<Texture> texture, const TextureBinding &binding) = 0;

        /// @brief A pure virtual method that returns a const string reference containing the API specific source code of the vertex shader
        /// @return A const string reference to a string containing the vertex shader source code
        virtual const std::string &GetVertexShaderSource() = 0;

        /// @brief A pure virtual method that returns a const string reference containing the API specific source code of the fragment shader
        /// @return A const string reference to a string containing the fragment shader source code
        virtual const std::string &GetFragmentShaderSource() = 0;

        /// @brief A pure virtual method that returns a const reference to the layout of the vertex buffer
        /// @return A const reference to a VertexBufferLayout containing the layout of the vertex buffer
        virtual const VertexBufferLayout &GetLayout() const = 0;

        /// @brief A pure virtual method that binds a uniform buffer to a shader
        /// @param buffer A uniform buffer to bind to the shader
        /// @param binding The shader that the uniform buffer should be bound to
        virtual void BindUniformBuffer(Ref<DeviceBuffer> buffer, const UniformResourceBinding &binding) = 0;
    };
}