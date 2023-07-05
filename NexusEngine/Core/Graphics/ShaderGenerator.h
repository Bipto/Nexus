#pragma once

#include <string>
#include <inttypes.h>

namespace Nexus::Graphics
{
    /// @brief An enum representing the type of shader
    enum class ShaderType
    {
        /// @brief Value representing an invalid shader type
        None = -1,

        /// @brief Value representing a vertex shader type
        Vertex = 0,

        /// @brief Value representing a fragment shader type
        Fragment = 1
    };

    /// @brief An enum representing different shader languages
    enum class ShaderLanguage
    {
        /// @brief Value representing the GLSL shader language
        GLSL,

        /// @brief Value representing the GLSLES shader language
        GLSLES,

        /// @brief Value representing the HLSL shader language
        HLSL
    };

#ifndef __EMSCRIPTEN__

    /// @brief A struct representing a set of options when generating a shader
    struct ShaderGenerationOptions
    {
        /// @brief The type of shader that should be generated
        ShaderType Type;

        /// @brief The shader language to generate from the input text
        ShaderLanguage OutputFormat;

        /// @brief The name of the shader
        std::string ShaderName;
    };

    /// @brief A struct representing the output of a shader generation
    struct CompilationResult
    {
        /// @brief A boolean value representing whether the generation was successful or not
        bool Successful;

        /// @brief A string containing the outuput text of the shader in the language selected
        std::string Source;

        /// @brief A string containing an error message if an error was encountered
        std::string Error;

        /// @brief The shader language that has been output by the generation
        ShaderLanguage OutputFormat;
    };

    /// @brief A class representing a shader generator instance
    class ShaderGenerator
    {
    public:
        /// @brief A method that converts a shader input text string into the selected shader language
        /// @param source An input string containing shader code to convert
        /// @param options A set of configuration options to use when generating a native shader string
        /// @return A compilation result returning the output of the shader generation
        CompilationResult Generate(const std::string &source, ShaderGenerationOptions options);
    };

#endif
}