#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Utils
{
/// @brief A struct used to store the source file of a vertex and fragment shader
struct ShaderSources
{
    /// @brief The source file of a vertex shader
    std::string VertexSource;

    /// @brief The source file of a fragment shader
    std::string FragmentSource;
};

/// @brief A method to parse a shader in a custom format and return a vertex and fragment shader
/// @param path The path to the file to read
/// @return A struct containing a vertex and fragment shader
static ShaderSources ParseCustomShaderFile(const std::string &path)
{
    std::ifstream stream(path);

    enum class ShaderType
    {
        None = -1,
        Vertex = 0,
        Fragment = 1,
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::None;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::Vertex;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::Fragment;
        }
        else
        {
            ss[(int)type] << line << "\n";
        }
    }

    ShaderSources source;
    source.VertexSource = ss[0].str();
    source.FragmentSource = ss[1].str();
    return source;
}
} // namespace Nexus::Utils