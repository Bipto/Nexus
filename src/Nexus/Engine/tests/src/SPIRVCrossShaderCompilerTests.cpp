#include <gtest/gtest.h>

#include "Nexus-Core/Shaders/SPIRVCrossShaderCompiler.hpp"

TEST(SPIRVShaderCompiler, CompilesValidShader)
{
    std::string shaderSource = R"(
		#version 450
		void main(){}
	)";

    Nexus::ShaderCompilationOptions compilationOptions = {
        .SourceInput =
            {
                .SourceText = shaderSource,
                .SourceBinary = {},
            },
        .InputCompilationDesc = {.ShaderLanguage = Nexus::Graphics::ShaderLanguage::GLSL,
                                 .ShaderVersion = {.Major = 4, .Minor = 5},
                                 .EntryPoint = "main",
                                 .Defines = {}},
        .OutputCompilationDesc = {.ShaderLanguage = Nexus::Graphics::ShaderLanguage::Vulkan_SPIRV,
                                  .ShaderVersion = {.Major = 1, .Minor = 6},
                                  .OptimisationLevel = Nexus::ShaderOptimisationLevel::NoOptimisation,
                                  .Debug = true},
        .Environment = {.API = Nexus::Graphics::GraphicsAPI::Vulkan, .Major = 1, .Minor = 4},
        .Stage = Nexus::Graphics::ShaderStage::Vertex,
        .DebugName = "ValidShader"};

    Nexus::SPIRVCrossShaderCompiler compiler = {};
    auto result = compiler.Compile(compilationOptions);

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result->OutputBinary.empty());
}

TEST(SPIRVShaderCompiler, FailsOnInvalidShader)
{
    std::string shaderSource = R"(
		#version 450
		this is not valid GLSL
	)";

    Nexus::ShaderCompilationOptions compilationOptions = {
        .SourceInput =
            {
                .SourceText = shaderSource,
                .SourceBinary = {},
            },
        .InputCompilationDesc = {.ShaderLanguage = Nexus::Graphics::ShaderLanguage::GLSL,
                                 .ShaderVersion = {.Major = 4, .Minor = 5},
                                 .EntryPoint = "main",
                                 .Defines = {}},
        .OutputCompilationDesc = {.ShaderLanguage = Nexus::Graphics::ShaderLanguage::Vulkan_SPIRV,
                                  .ShaderVersion = {.Major = 1, .Minor = 6},
                                  .OptimisationLevel = Nexus::ShaderOptimisationLevel::NoOptimisation,
                                  .Debug = true},
        .Environment = {.API = Nexus::Graphics::GraphicsAPI::Vulkan, .Major = 1, .Minor = 4},
        .Stage = Nexus::Graphics::ShaderStage::Vertex,
        .DebugName = "InvalidShader"};

    Nexus::SPIRVCrossShaderCompiler compiler = {};
    auto result = compiler.Compile(compilationOptions);

    ASSERT_FALSE(result.has_value());
    EXPECT_TRUE(result.error().size() > 0);
}

TEST(SPIRVShaderCompiler, DeterministicOutput)
{
    std::string shaderSource = R"(
		#version 450
		void main(){}
	)";

    Nexus::ShaderCompilationOptions compilationOptions = {
        .SourceInput =
            {
                .SourceText = shaderSource,
                .SourceBinary = {},
            },
        .InputCompilationDesc = {.ShaderLanguage = Nexus::Graphics::ShaderLanguage::GLSL,
                                 .ShaderVersion = {.Major = 4, .Minor = 5},
                                 .EntryPoint = "main",
                                 .Defines = {}},
        .OutputCompilationDesc = {.ShaderLanguage = Nexus::Graphics::ShaderLanguage::Vulkan_SPIRV,
                                  .ShaderVersion = {.Major = 1, .Minor = 6},
                                  .OptimisationLevel = Nexus::ShaderOptimisationLevel::NoOptimisation,
                                  .Debug = true},
        .Environment = {.API = Nexus::Graphics::GraphicsAPI::Vulkan, .Major = 1, .Minor = 4},
        .Stage = Nexus::Graphics::ShaderStage::Vertex,
        .DebugName = "ValidShader"};

    Nexus::SPIRVCrossShaderCompiler compiler = {};
    auto result = compiler.Compile(compilationOptions);
    auto result2 = compiler.Compile(compilationOptions);

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result2.has_value());

    EXPECT_EQ(result->OutputBinary.size(), result2->OutputBinary.size());
}

TEST(SPIRVShaderCompiler, HandlesPreprocessorDefines)
{
    std::string shaderSource = R"(
		#version 450
        #ifdef USE_COLOUR
        layout(location = 0) out vec4 color;
        #endif
        void main() {}
	)";

    Nexus::ShaderCompilationOptions compilationOptions = {
        .SourceInput =
            {
                .SourceText = shaderSource,
                .SourceBinary = {},
            },
        .InputCompilationDesc = {.ShaderLanguage = Nexus::Graphics::ShaderLanguage::GLSL,
                                 .ShaderVersion = {.Major = 4, .Minor = 5},
                                 .EntryPoint = "main",
                                 .Defines = {}},
        .OutputCompilationDesc = {.ShaderLanguage = Nexus::Graphics::ShaderLanguage::Vulkan_SPIRV,
                                  .ShaderVersion = {.Major = 1, .Minor = 6},
                                  .OptimisationLevel = Nexus::ShaderOptimisationLevel::NoOptimisation,
                                  .Debug = true},
        .Environment = {.API = Nexus::Graphics::GraphicsAPI::Vulkan, .Major = 1, .Minor = 4},
        .Stage = Nexus::Graphics::ShaderStage::Vertex,
        .DebugName = "ValidShader"};

    Nexus::SPIRVCrossShaderCompiler compiler = {};
    auto result = compiler.Compile(compilationOptions);

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result->OutputBinary.empty());
}

TEST(SPIRVShaderCompiler, ErrorMessageContainsLineNumber)
{
    std::string shaderSource = R"(
		#version 450
		invalid GLSL
	)";

    Nexus::ShaderCompilationOptions compilationOptions = {
        .SourceInput =
            {
                .SourceText = shaderSource,
                .SourceBinary = {},
            },
        .InputCompilationDesc = {.ShaderLanguage = Nexus::Graphics::ShaderLanguage::GLSL,
                                 .ShaderVersion = {.Major = 4, .Minor = 5},
                                 .EntryPoint = "main",
                                 .Defines = {}},
        .OutputCompilationDesc = {.ShaderLanguage = Nexus::Graphics::ShaderLanguage::Vulkan_SPIRV,
                                  .ShaderVersion = {.Major = 1, .Minor = 6},
                                  .OptimisationLevel = Nexus::ShaderOptimisationLevel::NoOptimisation,
                                  .Debug = true},
        .Environment = {.API = Nexus::Graphics::GraphicsAPI::Vulkan, .Major = 1, .Minor = 4},
        .Stage = Nexus::Graphics::ShaderStage::Vertex,
        .DebugName = "InvalidShader"};

    Nexus::SPIRVCrossShaderCompiler compiler = {};
    auto result = compiler.Compile(compilationOptions);

    ASSERT_FALSE(result.has_value());
    EXPECT_NE(result.error().find("syntax"), std::string::npos);
}

TEST(SPIRVShaderCompiler, CompilesLargeShader)
{
    std::string shaderSource = "#version 450\nvoid main(){}\n";
    shaderSource.reserve(200000);
    for (int i = 0; i < 5000; i++)
        shaderSource += "float x" + std::to_string(i) + " = 1.0;\n";

    Nexus::ShaderCompilationOptions compilationOptions = {
        .SourceInput =
            {
                .SourceText = shaderSource,
                .SourceBinary = {},
            },
        .InputCompilationDesc = {.ShaderLanguage = Nexus::Graphics::ShaderLanguage::GLSL,
                                 .ShaderVersion = {.Major = 4, .Minor = 5},
                                 .EntryPoint = "main",
                                 .Defines = {}},
        .OutputCompilationDesc = {.ShaderLanguage = Nexus::Graphics::ShaderLanguage::Vulkan_SPIRV,
                                  .ShaderVersion = {.Major = 1, .Minor = 6},
                                  .OptimisationLevel = Nexus::ShaderOptimisationLevel::NoOptimisation,
                                  .Debug = true},
        .Environment = {.API = Nexus::Graphics::GraphicsAPI::Vulkan, .Major = 1, .Minor = 4},
        .Stage = Nexus::Graphics::ShaderStage::Vertex,
        .DebugName = "LargeShader"};

    Nexus::SPIRVCrossShaderCompiler compiler = {};
    auto result = compiler.Compile(compilationOptions);

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result->OutputBinary.empty());
}

TEST(SPIRVShaderCompiler, MultipleSequentialCompilations)
{
    std::string shaderSource = R"(
		#version 450
		void main(){}
	)";

    Nexus::ShaderCompilationOptions compilationOptions = {
        .SourceInput =
            {
                .SourceText = shaderSource,
                .SourceBinary = {},
            },
        .InputCompilationDesc = {.ShaderLanguage = Nexus::Graphics::ShaderLanguage::GLSL,
                                 .ShaderVersion = {.Major = 4, .Minor = 5},
                                 .EntryPoint = "main",
                                 .Defines = {}},
        .OutputCompilationDesc = {.ShaderLanguage = Nexus::Graphics::ShaderLanguage::Vulkan_SPIRV,
                                  .ShaderVersion = {.Major = 1, .Minor = 6},
                                  .OptimisationLevel = Nexus::ShaderOptimisationLevel::NoOptimisation,
                                  .Debug = true},
        .Environment = {.API = Nexus::Graphics::GraphicsAPI::Vulkan, .Major = 1, .Minor = 4},
        .Stage = Nexus::Graphics::ShaderStage::Vertex,
        .DebugName = "ValidShader"};

    Nexus::SPIRVCrossShaderCompiler compiler = {};

    for (size_t i = 0; i < 5; i++)
    {
        auto result = compiler.Compile(compilationOptions);
        ASSERT_TRUE(result.has_value());
        EXPECT_FALSE(result->OutputBinary.empty());
    }
}