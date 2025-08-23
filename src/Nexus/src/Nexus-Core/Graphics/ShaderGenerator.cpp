#include "Nexus-Core/Graphics/ShaderGenerator.hpp"

#include "Nexus-Core/Graphics/ResourceSet.hpp"
#include "Nexus-Core/Graphics/ShaderDataType.hpp"
#include "Nexus-Core/Logging/Log.hpp"
#include "shaderc/shaderc.hpp"
#include "spirv_glsl.hpp"
#include "spirv_hlsl.hpp"

spv::ExecutionModel GetShaderExecutionModel(Nexus::Graphics::ShaderStage stage)
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
		default: throw std::runtime_error("Failed to find a valid shader stage");
	}
}

namespace Nexus::Graphics
{
	shaderc_shader_kind GetTypeOfShader(ShaderStage stage)
	{
		switch (stage)
		{
			case ShaderStage::Compute: return shaderc_glsl_compute_shader;
			case ShaderStage::Fragment: return shaderc_glsl_fragment_shader;
			case ShaderStage::Geometry: return shaderc_glsl_geometry_shader;
			case ShaderStage::TessellationControl: return shaderc_glsl_tess_control_shader;
			case ShaderStage::TessellationEvaluation: return shaderc_glsl_tess_evaluation_shader;
			case ShaderStage::Vertex: return shaderc_glsl_vertex_shader;
			case ShaderStage::RayGeneration: return shaderc_glsl_raygen_shader;
			case ShaderStage::RayAnyHit: return shaderc_glsl_anyhit_shader;
			case ShaderStage::RayClosestHit: return shaderc_glsl_closesthit_shader;
			case ShaderStage::RayIntersection: return shaderc_glsl_intersection_shader;
			case ShaderStage::RayMiss: return shaderc_glsl_miss_shader;
			case ShaderStage::Mesh: return shaderc_glsl_mesh_shader;
			case ShaderStage::Task: return shaderc_glsl_task_shader;
			default: throw std::runtime_error("Failed to find a valid shader stage");
		}
	}

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

	CompilationResult ShaderGenerator::Generate(const std::string &source, ShaderGenerationOptions options)
	{
		CompilationResult output;
		output.Successful	= false;
		output.Source		= {};
		output.Error		= {};
		output.OutputFormat = options.OutputFormat;

		// compile to SPIR-V
		shaderc::Compiler		   compiler;
		auto					   shaderType = GetTypeOfShader(options.Stage);
		shaderc::CompileOptions	   compileOptions = {};

		// disables the generation of code using demote instead of discard
		if (options.OutputFormat == Graphics::ShaderLanguage::GLSL || options.OutputFormat == Graphics::ShaderLanguage::GLSLES)
		{
			compileOptions.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0);
			compileOptions.SetTargetSpirv(shaderc_spirv_version_1_0);
		}
		else
		{
			compileOptions.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
			compileOptions.SetTargetSpirv(shaderc_spirv_version_1_6);
		}

		compileOptions.SetGenerateDebugInfo();

		shaderc::CompilationResult result = compiler.CompileGlslToSpv(source, shaderType, options.ShaderName.c_str(), compileOptions);

		if (result.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			output.Error += result.GetErrorMessage();
			return output;
		}

		std::vector<uint32_t> spirv_binary = {result.begin(), result.end()};
		output.SpirvBinary				   = spirv_binary;

		spirv_cross::CompilerGLSL::Options glOptions;
		glOptions.emit_push_constant_as_uniform_buffer = true;
		glOptions.vulkan_semantics					   = false;

		// compile to shader language
		switch (options.OutputFormat)
		{
			case ShaderLanguage::GLSL:
			{
				spirv_cross::CompilerGLSL compiler(spirv_binary);
				glOptions.version = 450;
				glOptions.es	  = false;
				compiler.set_common_options(glOptions);
				output.Source = compiler.compile();
				break;
			}
			case ShaderLanguage::GLSLES:
			{
				spirv_cross::CompilerGLSL compiler(spirv_binary);
				glOptions.version = 300;
				glOptions.es	  = true;
				compiler.set_common_options(glOptions);
				output.Source = compiler.compile();
				break;
			}
			case ShaderLanguage::HLSL:
			{
				spirv_cross::CompilerHLSL compiler(spirv_binary);

				const std::string name = GetD3DShaderEntryPoint(options.Stage);
				compiler.rename_entry_point("main", name.c_str(), GetShaderExecutionModel(options.Stage));

				MaintainHLSLUniformNames(compiler);

				glOptions.version = 330;
				glOptions.es	  = false;
				compiler.set_common_options(glOptions);

				spirv_cross::CompilerHLSL::Options hlslOptions;
				// allow the main method to be renamed
				hlslOptions.use_entry_point_name = true;
				// modern HLSL
				hlslOptions.shader_model						  = 50;
				hlslOptions.flatten_matrix_vertex_input_semantics = true;
				hlslOptions.force_storage_buffer_as_uav			  = true;
				compiler.set_hlsl_options(hlslOptions);
				output.Source = compiler.compile();
				break;
			}
			case ShaderLanguage::SPIRV:
			{
				spirv_cross::CompilerGLSL compiler(spirv_binary);
				glOptions.version = 450;
				glOptions.es	  = false;
				compiler.set_common_options(glOptions);
				output.Source = source;
				break;
			}

			default: throw std::runtime_error("Failed to find a valid shader format");
		}

		if (options.OutputFormat == ShaderLanguage::SPIRV)
		{
			output.SpirvBinary = spirv_binary;
		}

		output.Successful = true;
		return output;
	}

	std::string ShaderLanguageToString(ShaderLanguage language)
	{
		switch (language)
		{
			case ShaderLanguage::GLSL: return "GLSL";
			case ShaderLanguage::GLSLES: return "GLSLES";
			case ShaderLanguage::HLSL: return "HLSL";
			case ShaderLanguage::SPIRV: return "SPIRV";
			default: throw std::runtime_error("Failed to find a valid ShaderLanguage");
		}

		return {};
	}

	std::string GetD3DShaderEntryPoint(Nexus::Graphics::ShaderStage stage)
	{
		switch (stage)
		{
			case Nexus::Graphics::ShaderStage::Compute: return "cs_main";
			case Nexus::Graphics::ShaderStage::Fragment: return "fs_main";
			case Nexus::Graphics::ShaderStage::Geometry: return "gs_main";
			case Nexus::Graphics::ShaderStage::TessellationControl: return "tcs_main";
			case Nexus::Graphics::ShaderStage::TessellationEvaluation: return "tes_main";
			case Nexus::Graphics::ShaderStage::Vertex: return "vs_main";
			case Nexus::Graphics::ShaderStage::Mesh: return "ms_main";
			case Nexus::Graphics::ShaderStage::Task: return "ts_main";
			default: throw std::runtime_error("Failed to find a valid shader stage");
		}
	}
}	 // namespace Nexus::Graphics