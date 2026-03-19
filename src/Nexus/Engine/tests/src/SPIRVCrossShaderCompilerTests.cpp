#include <gtest/gtest.h>

#include "Nexus-Core/Shaders/SPIRVCrossShaderCompiler.hpp"

TEST(SPIRVShaderCompiler, CompilesValidShader)
{
	std::string shaderSource = R"(
		#version 450
		void main(){}
	)";

	Nexus::ShaderCompilationOptions compilationOptions = {
		.EntryPoint		   = "main",
		.TargetEnvironment = {.API = Nexus::Graphics::GraphicsAPI::Vulkan, .Major = 1, .Minor = 4},
		.Stage			   = Nexus::Graphics::ShaderStage::Vertex,
		.Defines		   = {},
		.InputLanguage	   = Nexus::Graphics::ShaderLanguage::GLSL,
		.InputVersion	   = {.Major = 4, .Minor = 5},
		.OutputLanguage	   = Nexus::Graphics::ShaderLanguage::Vulkan_SPIRV,
		.OutputVersion	   = {.Major = 1, .Minor = 6},
		.OptimisationLevel = Nexus::ShaderOptimisationLevel::None,
		.Debug			   = true,
	};

	Nexus::SPIRVCrossShaderCompiler compiler = {};
	auto							result	 = compiler.CompileShader(shaderSource, "ValidShader", compilationOptions);

	ASSERT_TRUE(result.has_value());
	EXPECT_FALSE(result->OutputSource.empty());
}

TEST(SPIRVShaderCompiler, FailsOnInvalidShader)
{
	std::string shaderSource = R"(
		#version 450
		this is not valid GLSL
	)";

	Nexus::ShaderCompilationOptions compilationOptions = {
		.EntryPoint		   = "main",
		.TargetEnvironment = {.API = Nexus::Graphics::GraphicsAPI::Vulkan, .Major = 1, .Minor = 4},
		.Stage			   = Nexus::Graphics::ShaderStage::Vertex,
		.Defines		   = {},
		.InputLanguage	   = Nexus::Graphics::ShaderLanguage::GLSL,
		.InputVersion	   = {.Major = 4, .Minor = 5},
		.OutputLanguage	   = Nexus::Graphics::ShaderLanguage::Vulkan_SPIRV,
		.OutputVersion	   = {.Major = 1, .Minor = 6},
		.OptimisationLevel = Nexus::ShaderOptimisationLevel::None,
		.Debug			   = true,
	};

	Nexus::SPIRVCrossShaderCompiler compiler = {};
	auto							result	 = compiler.CompileShader(shaderSource, "InvalidShader", compilationOptions);

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
		.EntryPoint		   = "main",
		.TargetEnvironment = {.API = Nexus::Graphics::GraphicsAPI::Vulkan, .Major = 1, .Minor = 4},
		.Stage			   = Nexus::Graphics::ShaderStage::Vertex,
		.Defines		   = {},
		.InputLanguage	   = Nexus::Graphics::ShaderLanguage::GLSL,
		.InputVersion	   = {.Major = 4, .Minor = 5},
		.OutputLanguage	   = Nexus::Graphics::ShaderLanguage::Vulkan_SPIRV,
		.OutputVersion	   = {.Major = 1, .Minor = 6},
		.OptimisationLevel = Nexus::ShaderOptimisationLevel::Performance,
		.Debug			   = false,
	};

	Nexus::SPIRVCrossShaderCompiler compiler = {};

	auto result	 = compiler.CompileShader(shaderSource, "ValidShader", compilationOptions);
	auto result2 = compiler.CompileShader(shaderSource, "ValidShader", compilationOptions);

	ASSERT_TRUE(result.has_value());
	ASSERT_TRUE(result2.has_value());

	EXPECT_EQ(result->OutputSource.size(), result2->OutputSource.size());
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
		.EntryPoint		   = "main",
		.TargetEnvironment = {.API = Nexus::Graphics::GraphicsAPI::Vulkan, .Major = 1, .Minor = 4},
		.Stage			   = Nexus::Graphics::ShaderStage::Vertex,
		.Defines		   = {Nexus::ShaderDefine {.Name = "USE_COLOUR", .Value = "1"}},
		.InputLanguage	   = Nexus::Graphics::ShaderLanguage::GLSL,
		.InputVersion	   = {.Major = 4, .Minor = 5},
		.OutputLanguage	   = Nexus::Graphics::ShaderLanguage::Vulkan_SPIRV,
		.OutputVersion	   = {.Major = 1, .Minor = 6},
		.OptimisationLevel = Nexus::ShaderOptimisationLevel::Performance,
		.Debug			   = false,
	};

	Nexus::SPIRVCrossShaderCompiler compiler = {};
	auto							result	 = compiler.CompileShader(shaderSource, "ValidShader", compilationOptions);

	ASSERT_TRUE(result.has_value());
	EXPECT_FALSE(result->OutputSource.empty());
}

TEST(SPIRVShaderCompiler, ErrorMessageContainsLineNumber)
{
	std::string shaderSource = R"(
		#version 450
		invalid GLSL
	)";

	Nexus::ShaderCompilationOptions compilationOptions = {
		.EntryPoint		   = "main",
		.TargetEnvironment = {.API = Nexus::Graphics::GraphicsAPI::Vulkan, .Major = 1, .Minor = 4},
		.Stage			   = Nexus::Graphics::ShaderStage::Vertex,
		.Defines		   = {Nexus::ShaderDefine {.Name = "USE_COLOUR", .Value = "1"}},
		.InputLanguage	   = Nexus::Graphics::ShaderLanguage::GLSL,
		.InputVersion	   = {.Major = 4, .Minor = 5},
		.OutputLanguage	   = Nexus::Graphics::ShaderLanguage::Vulkan_SPIRV,
		.OutputVersion	   = {.Major = 1, .Minor = 6},
		.OptimisationLevel = Nexus::ShaderOptimisationLevel::Performance,
		.Debug			   = false,
	};

	Nexus::SPIRVCrossShaderCompiler compiler = {};
	auto							result	 = compiler.CompileShader(shaderSource, "InvalidShader", compilationOptions);

	ASSERT_FALSE(result.has_value());
	EXPECT_NE(result.error().find("syntax"), std::string::npos);
}

TEST(SPIRVShaderCompiler, CompilesLargeShader)
{
	std::string shaderSource = "#version 450\nvoid main(){}\n";
	shaderSource.reserve(200000);
	for (int i = 0; i < 5000; i++) shaderSource += "float x" + std::to_string(i) + " = 1.0;\n";

	Nexus::ShaderCompilationOptions compilationOptions = {
		.EntryPoint		   = "main",
		.TargetEnvironment = {.API = Nexus::Graphics::GraphicsAPI::Vulkan, .Major = 1, .Minor = 4},
		.Stage			   = Nexus::Graphics::ShaderStage::Vertex,
		.Defines		   = {Nexus::ShaderDefine {.Name = "USE_COLOUR", .Value = "1"}},
		.InputLanguage	   = Nexus::Graphics::ShaderLanguage::GLSL,
		.InputVersion	   = {.Major = 4, .Minor = 5},
		.OutputLanguage	   = Nexus::Graphics::ShaderLanguage::Vulkan_SPIRV,
		.OutputVersion	   = {.Major = 1, .Minor = 6},
		.OptimisationLevel = Nexus::ShaderOptimisationLevel::Performance,
		.Debug			   = false,
	};

	Nexus::SPIRVCrossShaderCompiler compiler = {};
	auto							result	 = compiler.CompileShader(shaderSource, "LargeShader", compilationOptions);

	ASSERT_TRUE(result.has_value());
	EXPECT_FALSE(result->OutputSource.empty());
}

TEST(SPIRVShaderCompiler, MultipleSequentialCompilations)
{
	std::string shaderSource = R"(
		#version 450
		void main(){}
	)";

	Nexus::ShaderCompilationOptions compilationOptions = {
		.EntryPoint		   = "main",
		.TargetEnvironment = {.API = Nexus::Graphics::GraphicsAPI::Vulkan, .Major = 1, .Minor = 4},
		.Stage			   = Nexus::Graphics::ShaderStage::Vertex,
		.Defines		   = {},
		.InputLanguage	   = Nexus::Graphics::ShaderLanguage::GLSL,
		.InputVersion	   = {.Major = 4, .Minor = 5},
		.OutputLanguage	   = Nexus::Graphics::ShaderLanguage::Vulkan_SPIRV,
		.OutputVersion	   = {.Major = 1, .Minor = 6},
		.OptimisationLevel = Nexus::ShaderOptimisationLevel::None,
		.Debug			   = true,
	};

	Nexus::SPIRVCrossShaderCompiler compiler = {};

	for (size_t i = 0; i < 5; i++)
	{
		auto result = compiler.CompileShader(shaderSource, "ValidShader", compilationOptions);
		ASSERT_TRUE(result.has_value());
		EXPECT_FALSE(result->OutputSource.empty());
	}
}