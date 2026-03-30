#include "Nexus-Core/Shaders/SPIRVCrossShaderCompiler.hpp"
#include "Nexus-Core/Shaders/IShaderCompiler.hpp"

#include <cstddef>
#include <cstdint>
#include <expected>
#include <format>
#include <span>
#include <string.h>
#include <string>
#include <string_view>
#include <vector>

#include "RHI/APIVersion.hpp"
#include "RHI/GraphicsAPICreateInfo.hpp"
#include "RHI/ShaderLanguage.hpp"
#include "RHI/ShaderReflectionData.hpp"
#include "RHI/ShaderResources.hpp"

#include "Nexus-Core/Resources/IResourceLoader.hpp"

#include <shaderc/env.h>
#include <shaderc/shaderc.h>
#include <shaderc/shaderc.hpp>
#include <shaderc/status.h>
#include <spirv.hpp>
#include <spirv_cross_error_handling.hpp>
#include <spirv_glsl.hpp>
#include <spirv_hlsl.hpp>

namespace
{
	void SetHLSLUniformNames(spirv_cross::CompilerHLSL &compiler, const spirv_cross::SmallVector<spirv_cross::Resource> &resources)
	{
		for (const auto &resource : resources) { compiler.set_name(resource.id, resource.name); }
	}

	void MaintainHLSLUniformNames(spirv_cross::CompilerHLSL &compiler)
	{
		const auto &resources = compiler.get_shader_resources();
		SetHLSLUniformNames(compiler, resources.uniform_buffers);
		SetHLSLUniformNames(compiler, resources.storage_buffers);
		SetHLSLUniformNames(compiler, resources.stage_inputs);
		SetHLSLUniformNames(compiler, resources.stage_outputs);
		SetHLSLUniformNames(compiler, resources.subpass_inputs);
		SetHLSLUniformNames(compiler, resources.storage_images);
		SetHLSLUniformNames(compiler, resources.sampled_images);
		SetHLSLUniformNames(compiler, resources.atomic_counters);
		SetHLSLUniformNames(compiler, resources.acceleration_structures);
		SetHLSLUniformNames(compiler, resources.push_constant_buffers);
		SetHLSLUniformNames(compiler, resources.shader_record_buffers);
		SetHLSLUniformNames(compiler, resources.separate_images);
		SetHLSLUniformNames(compiler, resources.separate_samplers);
	}

	std::expected<spv::ExecutionModel, std::string> GetShaderExecutionModelFromShaderStage(Nexus::Graphics::ShaderStage stage)
	{
		switch (stage)
		{
			case Nexus::Graphics::ShaderStage::Compute: return spv::ExecutionModel::ExecutionModelGLCompute;
			case Nexus::Graphics::ShaderStage::Fragment: return spv::ExecutionModel::ExecutionModelFragment;
			case Nexus::Graphics::ShaderStage::Geometry: return spv::ExecutionModel::ExecutionModelGeometry;
			case Nexus::Graphics::ShaderStage::TessellationControl: return spv::ExecutionModel::ExecutionModelTessellationControl;
			case Nexus::Graphics::ShaderStage::TessellationEvaluation: return spv::ExecutionModel::ExecutionModelTessellationEvaluation;
			case Nexus::Graphics::ShaderStage::Vertex: return spv::ExecutionModel::ExecutionModelVertex;
			case Nexus::Graphics::ShaderStage::RayGeneration: return spv::ExecutionModel::ExecutionModelRayGenerationKHR;
			case Nexus::Graphics::ShaderStage::RayAnyHit: return spv::ExecutionModel::ExecutionModelAnyHitKHR;
			case Nexus::Graphics::ShaderStage::RayClosestHit: return spv::ExecutionModel::ExecutionModelClosestHitKHR;
			case Nexus::Graphics::ShaderStage::RayIntersection: return spv::ExecutionModel::ExecutionModelIntersectionKHR;
			case Nexus::Graphics::ShaderStage::RayMiss: return spv::ExecutionModel::ExecutionModelMissKHR;
			case Nexus::Graphics::ShaderStage::Mesh: return spv::ExecutionModel::ExecutionModelMeshEXT;
			case Nexus::Graphics::ShaderStage::Task: return spv::ExecutionModel::ExecutionModelTaskEXT;
			default: return std::unexpected("Failed to find a valid shader stage");
		}
	}

	std::expected<shaderc_shader_kind, std::string> GetTypeOfShaderFromShaderStage(Nexus::Graphics::ShaderStage stage)
	{
		switch (stage)
		{
			case Nexus::Graphics::ShaderStage::Compute: return shaderc_glsl_compute_shader;
			case Nexus::Graphics::ShaderStage::Fragment: return shaderc_glsl_fragment_shader;
			case Nexus::Graphics::ShaderStage::Geometry: return shaderc_glsl_geometry_shader;
			case Nexus::Graphics::ShaderStage::TessellationControl: return shaderc_glsl_tess_control_shader;
			case Nexus::Graphics::ShaderStage::TessellationEvaluation: return shaderc_glsl_tess_evaluation_shader;
			case Nexus::Graphics::ShaderStage::Vertex: return shaderc_glsl_vertex_shader;
			case Nexus::Graphics::ShaderStage::RayGeneration: return shaderc_glsl_raygen_shader;
			case Nexus::Graphics::ShaderStage::RayAnyHit: return shaderc_glsl_anyhit_shader;
			case Nexus::Graphics::ShaderStage::RayClosestHit: return shaderc_glsl_closesthit_shader;
			case Nexus::Graphics::ShaderStage::RayIntersection: return shaderc_glsl_intersection_shader;
			case Nexus::Graphics::ShaderStage::RayMiss: return shaderc_glsl_miss_shader;
			case Nexus::Graphics::ShaderStage::Mesh: return shaderc_glsl_mesh_shader;
			case Nexus::Graphics::ShaderStage::Task: return shaderc_glsl_task_shader;
			default: return std::unexpected("Failed to find a valid shader stage");
		}
	}

	struct ShaderCEnvironmentInfo
	{
		shaderc_target_env	targetEnv;
		shaderc_env_version targetVersion;
	};

	std::expected<ShaderCEnvironmentInfo, std::string> GetShaderCTargetEnv(const Nexus::ShaderCompilationOptions &options)
	{
		switch (options.Environment.API)
		{
			case Nexus::Graphics::GraphicsAPI::Vulkan:
			{
				if (options.Environment.Major == 1)
				{
					switch (options.Environment.Minor)
					{
						case 0:
							return ShaderCEnvironmentInfo {.targetEnv = shaderc_target_env_vulkan, .targetVersion = shaderc_env_version_vulkan_1_0};
						case 1:
							return ShaderCEnvironmentInfo {.targetEnv = shaderc_target_env_vulkan, .targetVersion = shaderc_env_version_vulkan_1_1};
						case 2:
							return ShaderCEnvironmentInfo {.targetEnv = shaderc_target_env_vulkan, .targetVersion = shaderc_env_version_vulkan_1_2};
						case 3:
							return ShaderCEnvironmentInfo {.targetEnv = shaderc_target_env_vulkan, .targetVersion = shaderc_env_version_vulkan_1_3};
						case 4:
							return ShaderCEnvironmentInfo {.targetEnv = shaderc_target_env_vulkan, .targetVersion = shaderc_env_version_vulkan_1_4};
						default: return std::unexpected("Failed to find a valid Vulkan minor version");
					}
				}
				else
				{
					return std::unexpected("Failed to find a valid Vulkan major version");
				}
			}
			case Nexus::Graphics::GraphicsAPI::OpenGL:
			{
				// if we have modern OpenGL, we only need to support core functionality in the shaders
				if (options.Environment.Major > 3 && options.Environment.Minor > 3)
				{
					return ShaderCEnvironmentInfo {
						.targetEnv	   = shaderc_target_env_opengl,
						.targetVersion = shaderc_env_version_opengl_4_5,
					};
				}
				// otherwise, we need to use compatibility support
				else
				{
					return ShaderCEnvironmentInfo {
						.targetEnv	   = shaderc_target_env_opengl_compat,
						.targetVersion = shaderc_env_version_opengl_4_5,
					};
				}
			}
			default:
			{
				return ShaderCEnvironmentInfo {.targetEnv = shaderc_target_env_default, .targetVersion = shaderc_env_version_vulkan_1_0};
			}
		}
	}

	std::expected<shaderc_spirv_version, std::string> GetSpirvShaderVersion(const Nexus::ShaderCompilationOptions &options)
	{
		if (options.OutputCompilationDesc.ShaderLanguage == Nexus::Graphics::ShaderLanguage::Vulkan_SPIRV)
		{
			if (options.OutputCompilationDesc.ShaderVersion.Major == 1)
			{
				switch (options.OutputCompilationDesc.ShaderVersion.Minor)
				{
					case 0: return shaderc_spirv_version_1_0;
					case 1: return shaderc_spirv_version_1_1;
					case 2: return shaderc_spirv_version_1_2;
					case 3: return shaderc_spirv_version_1_3;
					case 4: return shaderc_spirv_version_1_4;
					case 5: return shaderc_spirv_version_1_5;
					case 6: return shaderc_spirv_version_1_6;
					default: return std::unexpected("Invalid SPIRV Minor Version");
				}
			}
			else
			{
				return std::unexpected("Invalid SPIRV Major Version");
			}
		}
		else
		{
			return shaderc_spirv_version_1_0;
		}
	}

	std::expected<shaderc_optimization_level, std::string> GetShadercOptimisationLevel(Nexus::ShaderOptimisationLevel level)
	{
		switch (level)
		{
			case Nexus::ShaderOptimisationLevel::None: return shaderc_optimization_level_zero;
			case Nexus::ShaderOptimisationLevel::Size: return shaderc_optimization_level_size;
			case Nexus::ShaderOptimisationLevel::Performance: return shaderc_optimization_level_performance;
			default: return std::unexpected("Failed to find a valid optimisation level");
		}
	}

	std::expected<shaderc_source_language, std::string> SetShadercInputLanguageInfo(Nexus::Graphics::ShaderLanguage inputLanguage)
	{
		switch (inputLanguage)
		{
			case Nexus::Graphics::ShaderLanguage::GLSL:
			case Nexus::Graphics::ShaderLanguage::GLSLES:
			{
				return shaderc_source_language_glsl;
			}
			case Nexus::Graphics::ShaderLanguage::HLSL:
			{
				return shaderc_source_language_hlsl;
			}
			default: return std::unexpected("Failed to find a valid input shader language");
		}
	}

	int ExtractGLSLShaderProfileVersion(Nexus::Graphics::ShaderVersion version)
	{
		// e.g. GLSL 4.5 becomes 450
		return (version.Major * 100) + (version.Minor * 10);
	}

	int ExtractHLSLShaderProfileVersion(Nexus::Graphics::ShaderVersion version)
	{
		// e.g. HLSL 6.1 becomes 61
		return (version.Major * 10) + version.Minor;
	}

	/// compiles a GLSL or HLSL string into SPIRV bytecode using the requested options
	std::expected<std::vector<std::byte>, std::string> CompileGLSL_HLSL_ToSPIRV(const std::string					  &source,
																				const std::string					  &shaderName,
																				const Nexus::ShaderCompilationOptions &options)
	{
		auto shaderEnvironment = GetShaderCTargetEnv(options);
		if (!shaderEnvironment)
		{
			return std::unexpected(shaderEnvironment.error());
		}

		auto spirvVersion = GetSpirvShaderVersion(options);
		if (!spirvVersion)
		{
			return std::unexpected(spirvVersion.error());
		}

		auto shaderKind = GetTypeOfShaderFromShaderStage(options.Stage);
		if (!shaderKind)
		{
			return std::unexpected(shaderKind.error());
		}

		auto optimisationLevel = GetShadercOptimisationLevel(options.OutputCompilationDesc.OptimisationLevel);
		if (!optimisationLevel)
		{
			return std::unexpected(optimisationLevel.error());
		}

		shaderc::Compiler		compiler	   = {};
		shaderc::CompileOptions compileOptions = {};
		compileOptions.SetTargetEnvironment(shaderEnvironment->targetEnv, shaderEnvironment->targetVersion);
		compileOptions.SetTargetSpirv(*spirvVersion);
		compileOptions.SetOptimizationLevel(*optimisationLevel);

		// preprocessor defines
		for (const auto &[name, value] : options.InputCompilationDesc.Defines)
		{
			if (value.has_value())
			{
				compileOptions.AddMacroDefinition(name, value.value());
			}
			else
			{
				compileOptions.AddMacroDefinition(name);
			}
		}

		if (options.OutputCompilationDesc.Debug)
		{
			compileOptions.SetGenerateDebugInfo();
		}

		if (options.InputCompilationDesc.ShaderLanguage == Nexus::Graphics::ShaderLanguage::GLSL)
		{
			int glslVersion = ExtractGLSLShaderProfileVersion(options.InputCompilationDesc.ShaderVersion);
			compileOptions.SetForcedVersionProfile(glslVersion, shaderc_profile_core);
		}
		else if (options.InputCompilationDesc.ShaderLanguage == Nexus::Graphics::ShaderLanguage::GLSLES)
		{
			int glslVersion = ExtractGLSLShaderProfileVersion(options.InputCompilationDesc.ShaderVersion);
			compileOptions.SetForcedVersionProfile(glslVersion, shaderc_profile_es);
		}

		shaderc::CompilationResult result = compiler.CompileGlslToSpv(source, *shaderKind, shaderName.c_str());
		if (result.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			return std::unexpected(result.GetErrorMessage());
		}

		std::vector<uint32_t>  spirvBinary = {result.begin(), result.end()};
		std::vector<std::byte> returnedSource(spirvBinary.size() * sizeof(uint32_t));
		memcpy(returnedSource.data(), spirvBinary.data(), returnedSource.size());

		return returnedSource;
	}

	std::expected<Nexus::ShaderCompilationResult, std::string> CompileSPIRVToGLSL(Nexus::Graphics::ShaderVersion shaderVersion,
																				  bool							 es,
																				  std::span<const std::byte>	 binaryData)
	{
		// check that the data does contain a valid amount of SPIR-V
		if (binaryData.size_bytes() / sizeof(uint32_t) != 0)
		{
			return std::unexpected("binaryData does not containg valid SPIR-V");
		}

		std::span<const uint32_t> spirvView = {reinterpret_cast<const uint32_t *>(binaryData.data()), binaryData.size_bytes() / sizeof(uint32_t)};

		int glslVersion = ExtractGLSLShaderProfileVersion(shaderVersion);

		spirv_cross::CompilerGLSL compiler(spirvView.data(), spirvView.size());

		spirv_cross::CompilerGLSL::Options options;
		options.emit_push_constant_as_uniform_buffer = true;
		options.vulkan_semantics					 = false;
		options.version								 = glslVersion;
		compiler.set_common_options(options);

		try
		{
			std::string shaderText = compiler.compile();
			return Nexus::ShaderCompilationResult {.OutputText = shaderText, .OutputBinary = {}, .ReflectionData = {}, .Warnings = {}};
		}
		catch (const spirv_cross::CompilerError &e)
		{
			std::string errorMessage = std::format("SPIRV-Cross error: {}\n", e.what());
			return std::unexpected(errorMessage);
		}
	}

	std::expected<Nexus::ShaderCompilationResult, std::string> CompileSPIRVToHLSL(Nexus::Graphics::ShaderVersion shaderVersion,
																				  std::span<const std::byte>	 binaryData)
	{
		// check that the data does contain a valid amount of SPIR-V
		if (binaryData.size_bytes() / sizeof(uint32_t) != 0)
		{
			return std::unexpected("binaryData does not containg valid SPIR-V");
		}

		std::span<const uint32_t> spirvView = {reinterpret_cast<const uint32_t *>(binaryData.data()), binaryData.size_bytes() / sizeof(uint32_t)};

		int hlslVersion = ExtractHLSLShaderProfileVersion(shaderVersion);

		spirv_cross::CompilerHLSL compiler(spirvView.data(), spirvView.size());

		MaintainHLSLUniformNames(compiler);

		spirv_cross::CompilerHLSL::Options options;
		options.use_entry_point_name				  = true;
		options.shader_model						  = hlslVersion;
		options.flatten_matrix_vertex_input_semantics = true;
		options.force_storage_buffer_as_uav			  = true;
		compiler.set_hlsl_options(options);

		try
		{
			std::string shaderText = compiler.compile();
			return Nexus::ShaderCompilationResult {.OutputText = shaderText, .OutputBinary = {}, .ReflectionData = {}, .Warnings = {}};
		}
		catch (const spirv_cross::CompilerError &e)
		{
			std::string errorMessage = std::format("SPIRV-Cross error: {}\n", e.what());
			return std::unexpected(errorMessage);
		}
	}

}	 // namespace

namespace Nexus
{
	std::expected<ShaderCompilationResult, std::string> SPIRVCrossShaderCompiler::Compile(const ShaderCompilationOptions &options) const
	{
		switch (options.OutputCompilationDesc.ShaderLanguage)
		{
			case Graphics::ShaderLanguage::OpenGL_SPIRV:
			case Graphics::ShaderLanguage::Vulkan_SPIRV:
			{
				if (options.InputCompilationDesc.ShaderLanguage == Graphics::ShaderLanguage::GLSL ||
					options.InputCompilationDesc.ShaderLanguage == Graphics::ShaderLanguage::GLSLES ||
					options.InputCompilationDesc.ShaderLanguage == Graphics::ShaderLanguage::HLSL)
				{
					std::string source = std::string(options.SourceInput.SourceText);
					std::string name   = std::string(options.DebugName);
					auto		result = CompileGLSL_HLSL_ToSPIRV(source, name, options);

					if (result)
					{
						return ShaderCompilationResult {.OutputText = {}, .OutputBinary = *result, .ReflectionData = {}, .Warnings = {}};
					}
					else
					{
						return std::unexpected(result.error());
					}
				}
				else
				{
					return std::unexpected("Unsupported input language");
				}
			}
			case Graphics::ShaderLanguage::GLSL:
			{
				return CompileSPIRVToGLSL(options.OutputCompilationDesc.ShaderVersion, false, options.SourceInput.SourceBinary);
			}
			case Graphics::ShaderLanguage::GLSLES:
			{
				return CompileSPIRVToGLSL(options.OutputCompilationDesc.ShaderVersion, true, options.SourceInput.SourceBinary);
			}
			case Graphics::ShaderLanguage::HLSL:
			{
				return CompileSPIRVToHLSL(options.OutputCompilationDesc.ShaderVersion, options.SourceInput.SourceBinary);
			}
			default: return std::unexpected("Failed to find a supported output language");
		}

		return std::unexpected("Failed to compile shader");
	}
}	 // namespace Nexus