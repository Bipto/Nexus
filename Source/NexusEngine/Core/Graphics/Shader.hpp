#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Texture.hpp"
#include "Core/Memory.hpp"
#include "Buffer.hpp"
#include "Core/Vertex.hpp"
#include "Core/Graphics/ResourceSet.hpp"

namespace Nexus::Graphics
{
    /// @brief A pure virtual class representing an API specific shader
    class Shader
    {
    public:
        /// @brief A pure virtual method that returns a const string reference containing the API specific source code of the vertex shader
        /// @return A const string reference to a string containing the vertex shader source code
        virtual const std::string &GetVertexShaderSource() = 0;

        /// @brief A pure virtual method that returns a const string reference containing the API specific source code of the fragment shader
        /// @return A const string reference to a string containing the fragment shader source code
        virtual const std::string &GetFragmentShaderSource() = 0;

        /// @brief A pure virtual method that returns a const reference to the layout of the vertex buffer
        /// @return A const reference to a VertexBufferLayout containing the layout of the vertex buffer
        virtual const VertexBufferLayout &GetLayout() const = 0;
    };
}