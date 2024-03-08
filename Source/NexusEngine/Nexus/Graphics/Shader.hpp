#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Texture.hpp"
#include "Nexus/Types.hpp"
#include "Buffer.hpp"
#include "Nexus/Vertex.hpp"
#include "Nexus/Graphics/ResourceSet.hpp"

namespace Nexus::Graphics
{
    /// @brief A pure virtual class representing an API specific shader
    class Shader
    {
    public:
        /// @brief A virtual destructor to allow resources to be cleaned up
        virtual ~Shader() {}

        /// @brief A pure virtual method that returns a const string reference containing the API specific source code of the vertex shader
        /// @return A const string reference to a string containing the vertex shader source code
        virtual const std::string &GetVertexShaderSource() = 0;

        /// @brief A pure virtual method that returns a const string reference containing the API specific source code of the fragment shader
        /// @return A const string reference to a string containing the fragment shader source code
        virtual const std::string &GetFragmentShaderSource() = 0;
    };
}