#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Texture.h"
#include "Core/Memory.h"
#include "Buffer.h"
#include "Core/Vertex.h"

namespace Nexus::Graphics
{
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