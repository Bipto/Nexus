#pragma once

#include <expected>
#include <string>
#include <unordered_set>

#include "Nexus-Core/Resources/IResourceLoader.hpp"

#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
    /// @brief A class that preprocesses a shader and includes all include paths
    class NX_API ShaderPreprocessor
    {
      public:
        /// @brief A shader preprocessor cannot be created without an implementation
        /// of a resource loader
        ShaderPreprocessor() = delete;

        /// @brief A constructor that creates a new resource loader
        /// @param loader A loader that is used to load any included files
        ShaderPreprocessor(IResourceLoader *loader);

        /// @brief A method that preprocesses a shader file
        /// @param shaderPath The path that the shader file was loaded from
        /// @param shaderText The unprocessed text of the shader
        /// @param includeDirectories Directories that should be searched to find any
        /// files that are included by the shader
        /// @return A string representing the fully processed shader that is ready to
        /// be used futher, or a string containing a description of what went wrong
        /// during the preprocessing stage
        std::expected<std::string, std::string> PreprocessShader(
            const std::string &shaderPath, const std::string &shaderText,
            const std::vector<std::string> &includeDirectories
        ) const;

      private:
        /// @brief A method that contains the implementation of the preprocessing
        /// stage for shaders
        /// @param shaderPath The path that the shader file was loaded from
        /// @param shaderText The unprocessed text of the shader
        /// @param includeDirectories Directories that should be searched to find any
        /// files that are included by the shader
        /// @param includeStack A list of all files that have been included to
        /// prevent circular dependencies
        /// @param onceIncluded A list of files that have already been included to
        /// prevent duplicated code
        /// @param currentFile A string containing the name of the current include
        /// file that is being processed
        /// @return A string representing the fully processed shader that is ready to
        /// be used futher, or a string containing a description of what went wrong
        /// during the preprocessing stage
        std::expected<std::string, std::string> PreprocessShader(
            const std::string &shaderPath, const std::string &shaderText,
            const std::vector<std::string> &includeDirectories,
            std::vector<std::string> &includeStack,
            std::unordered_set<std::string> &onceIncluded,
            const std::string &currentFile
        ) const;

      private:
        /// @brief A resource loader that is used to load any files included by the
        /// shader
        IResourceLoader *m_ResourceLoader = nullptr;
    };
} // namespace Nexus