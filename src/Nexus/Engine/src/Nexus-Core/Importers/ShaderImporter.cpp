#include "Nexus-Core/Importers/ShaderImporter.hpp"

#include "Nexus-Core/Shaders/SPIRVCrossShaderCompiler.hpp"
#include "Nexus-Core/Shaders/ShaderPreprocessor.hpp"

namespace Nexus
{
    std::expected<ShaderCompilationResult, std::string> ShaderImporter::LoadAndCompileShader(
        IResourceLoader *loader, std::string_view path, const ShaderCompilationInputDescription &shaderInputDescription,
        const ShaderCompilationOutputDescription &shaderOutputDescription,
        const std::vector<std::string> &shaderIncludeDirectories, Graphics::GraphicsAPIInfo environment,
        Graphics::ShaderStage stage, const std::string &debugName
    ) const
    {
        std::vector<std::string> includeDirectories = {};

        std::string shaderPathString = std::string{path};

        // loading a binary shader is simpler, as there is no preprocessing
        if (Graphics::IsBinaryShaderFormat(shaderInputDescription.ShaderLanguage))
        {
            return loader->LoadBytes(path).and_then([&](const std::vector<std::byte> &shaderData) {
                return CompileBinaryStep(
                    shaderData, shaderInputDescription, shaderOutputDescription, environment, stage, debugName
                );
            });
        }
        // we can only preprocess a text shader
        else
        {
            return loader->LoadString(path)
                .and_then([&](const std::string &shaderText) {
                    return PreprocessStep(loader, shaderPathString, shaderText, shaderIncludeDirectories);
                })
                .and_then([&](const std::string &shaderText) {
                    return CompileTextStep(
                        shaderText, shaderInputDescription, shaderOutputDescription, environment, stage, debugName
                    );
                });
        }
    }

    std::expected<std::string, std::string> ShaderImporter::PreprocessStep(
        IResourceLoader *loader, const std::string &shaderPath, const std::string &shaderText,
        const std::vector<std::string> &shaderIncludeDirectories
    ) const
    {
        ShaderPreprocessor preprocessor(loader);
        return preprocessor.PreprocessShader(shaderPath, shaderText, shaderIncludeDirectories);
    }

    std::expected<ShaderCompilationResult, std::string> ShaderImporter::CompileTextStep(
        const std::string &shaderText, const ShaderCompilationInputDescription &shaderInputDescription,
        const ShaderCompilationOutputDescription &shaderOutputDescription, Graphics::GraphicsAPIInfo environment,
        Graphics::ShaderStage stage, const std::string &debugName
    ) const
    {
        SPIRVCrossShaderCompiler shaderCompiler{};

        ShaderCompilationOptions compilationOptions = {};
        compilationOptions.SourceInput.SourceText = shaderText;
        compilationOptions.InputCompilationDesc = shaderInputDescription;
        compilationOptions.OutputCompilationDesc = shaderOutputDescription;
        compilationOptions.Environment = environment;
        compilationOptions.DebugName = debugName;
        compilationOptions.Stage = stage;

        return shaderCompiler.Compile(compilationOptions);
    }

    std::expected<ShaderCompilationResult, std::string> ShaderImporter::CompileBinaryStep(
        std::span<const std::byte> shaderBinary, const ShaderCompilationInputDescription &shaderInputDescription,
        const ShaderCompilationOutputDescription &shaderOutputDescription, Graphics::GraphicsAPIInfo environment,
        Graphics::ShaderStage stage, const std::string &debugName
    ) const
    {
        SPIRVCrossShaderCompiler shaderCompiler{};

        ShaderCompilationOptions compilationOptions = {};
        compilationOptions.SourceInput.SourceBinary = shaderBinary;
        compilationOptions.InputCompilationDesc = shaderInputDescription;
        compilationOptions.OutputCompilationDesc = shaderOutputDescription;
        compilationOptions.Environment = environment;
        compilationOptions.DebugName = debugName;
        compilationOptions.Stage = stage;

        return shaderCompiler.Compile(compilationOptions);
    }
} // namespace Nexus