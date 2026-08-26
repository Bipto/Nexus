#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "Nexus-Core/Resources/IResourceLoader.hpp"
#include "Nexus-Core/Shaders/IShaderCompiler.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
    class NX_API ShaderImporter
    {
      public:
        ShaderImporter() = default;

        [[nodiscard]] std::expected<ShaderCompilationResult, std::string> LoadAndCompileShader(
            IResourceLoader *loader, std::string_view path,
            const ShaderCompilationInputDescription &shaderInputDescription,
            const ShaderCompilationOutputDescription &shaderOutputDescription,
            const std::vector<std::string> &shaderIncludeDirectories, Graphics::GraphicsAPIInfo environment,
            Graphics::ShaderStage stage, const std::string &debugName) const;

      private:
        std::expected<std::string, std::string> PreprocessStep(
            IResourceLoader *loader, const std::string &shaderPath, const std::string &shaderText,
            const std::vector<std::string> &shaderIncludeDirectories) const;

        std::expected<ShaderCompilationResult, std::string> CompileTextStep(
            const std::string &shaderText, const ShaderCompilationInputDescription &shaderInputDescription,
            const ShaderCompilationOutputDescription &shaderOutputDescription, Graphics::GraphicsAPIInfo environment,
            Graphics::ShaderStage stage, const std::string &debugName) const;

        std::expected<ShaderCompilationResult, std::string> CompileBinaryStep(
            std::span<const std::byte> shaderBinary, const ShaderCompilationInputDescription &shaderInputDescription,
            const ShaderCompilationOutputDescription &shaderOutputDescription, Graphics::GraphicsAPIInfo environment,
            Graphics::ShaderStage stage, const std::string &debugName) const;
    };
} // namespace Nexus