#pragma once

#include <string>

#include "RHI/ShaderLanguage.hpp"
#include "RHI/ShaderModule.hpp"

namespace Nexus::Graphics
{

    /// @brief An enum representing the type of SPIRV code to emit
    enum class TargetEnvironment
    {
        Vulkan_1_0,
        Vulkan_1_1,
        Vulkan_1_2,
        Vulkan_1_3,
        OpenGL
    };

    /// @brief An enum representing the version of SPIRV to emit
    enum class SPIRV_Version
    {
        Version_1_0,
        Version_1_1,
        Version_1_2,
        Version_1_3,
        Version_1_4,
        Version_1_5,
        Version_1_6,
    };

    std::string ShaderLanguageToString(ShaderLanguage language);

    /// @brief A struct representing a set of options when generating a shader
    struct ShaderGenerationOptions
    {
        /// @brief The type of shader that should be generated
        ShaderStage Stage;

        /// @brief The shader language to generate from the input text
        ShaderLanguage OutputFormat;

        /// @brief The name of the shader
        std::string ShaderName;
    };

    /// @brief A struct representing the output of a shader generation
    struct CompilationResult
    {
        /// @brief A boolean value representing whether the generation was successful
        /// or not
        bool Successful;

        /// @brief A string containing the input text of the shader
        std::string Source;

        /// @brief A string containing the output text of the shader in the specified
        /// shader format
        std::string CompiledShader;

        /// @brief A string containing an error message if an error was encountered
        std::string Error;

        /// @brief The shader language that has been output by the generation
        ShaderLanguage OutputFormat;

        /// @brief A vector containing the SPIR-V binary bytecode
        std::vector<uint32_t> SpirvBinary;

        /// @brief A vector containing the input attributes of the shader
        std::vector<ShaderAttribute> InputAttributes;

        /// @brief A vector containing the output attributes of the shader
        std::vector<ShaderAttribute> OutputAttributes;
    };

    /// @brief A class representing a shader generator instance
    class ShaderGenerator
    {
      public:
        /// @brief A method that converts a shader input text string into the
        /// selected shader language
        /// @param source An input string containing shader code to convert
        /// @param options A set of configuration options to use when generating a
        /// native shader string
        /// @return A compilation result returning the output of the shader
        /// generation
        CompilationResult Generate(const std::string &source, ShaderGenerationOptions options);

        std::vector<uint32_t> GenerateSPIRV(
            const std::string &source, ShaderStage stage, TargetEnvironment env, SPIRV_Version version, bool debugInfo
        );
    };

    std::string GetD3DShaderEntryPoint(Nexus::Graphics::ShaderStage stage);
} // namespace Nexus::Graphics