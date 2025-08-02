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
		case Nexus::Graphics::ShaderStage::TesselationControl: return spv::ExecutionModel::ExecutionModelTessellationControl;
		case Nexus::Graphics::ShaderStage::TesselationEvaluation: return spv::ExecutionModel::ExecutionModelTessellationEvaluation;
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
			case ShaderStage::TesselationControl: return shaderc_glsl_tess_control_shader;
			case ShaderStage::TesselationEvaluation: return shaderc_glsl_tess_evaluation_shader;
			case ShaderStage::Vertex: return shaderc_glsl_vertex_shader;
			case Nexus::Graphics::ShaderStage::RayGeneration: return shaderc_glsl_raygen_shader;
			case Nexus::Graphics::ShaderStage::RayAnyHit: return shaderc_glsl_anyhit_shader;
			case Nexus::Graphics::ShaderStage::RayClosestHit: return shaderc_glsl_closesthit_shader;
			case Nexus::Graphics::ShaderStage::RayIntersection: return shaderc_glsl_intersection_shader;
			case Nexus::Graphics::ShaderStage::RayMiss: return shaderc_glsl_miss_shader;
			case ShaderStage::Mesh: return shaderc_glsl_mesh_shader;
			case ShaderStage::Task: return shaderc_glsl_task_shader;
			default: throw std::runtime_error("Failed to find a valid shader stage");
		}
	}

	void ToLinearResourceSet(spirv_cross::Compiler &compiler, ShaderLanguage language)
	{
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		// find all resources in shaders
		for (const auto &image : resources.sampled_images)
		{
			uint32_t set	 = compiler.get_decoration(image.id, spv::DecorationDescriptorSet);
			uint32_t binding = compiler.get_decoration(image.id, spv::DecorationBinding);

			// example of how to retrieve resource information
			/* const auto &baseType = compiler.get_type(image.base_type_id);
			const auto &type = compiler.get_type(image.type_id);
			if (type.image.dim == spv::Dim2D)
			{
			} */

			uint32_t slot = (set * ResourceSet::DescriptorSetCount) + binding;
			compiler.unset_decoration(image.id, spv::DecorationDescriptorSet);

			if (language == ShaderLanguage::GLSL | language == ShaderLanguage::GLSLES)
			{
				compiler.unset_decoration(image.id, spv::DecorationBinding);
			}
			else
			{
				compiler.set_decoration(image.id, spv::DecorationBinding, slot);
			}
		}

		for (const auto &uniformBuffer : resources.uniform_buffers)
		{
			uint32_t set	 = compiler.get_decoration(uniformBuffer.id, spv::DecorationDescriptorSet);
			uint32_t binding = compiler.get_decoration(uniformBuffer.id, spv::DecorationBinding);

			uint32_t slot = (set * ResourceSet::DescriptorSetCount) + binding;
			compiler.unset_decoration(uniformBuffer.id, spv::DecorationDescriptorSet);

			if (language == ShaderLanguage::GLSL || language == ShaderLanguage::GLSLES)
			{
				compiler.unset_decoration(uniformBuffer.id, spv::DecorationBinding);
			}
			else
			{
				compiler.set_decoration(uniformBuffer.id, spv::DecorationBinding, slot);
			}
		}
	}

	void CreateResourceSetSpecification(const spirv_cross::Compiler &compiler, ResourceSetSpecification &resources)
	{
		spirv_cross::ShaderResources shaderResources = compiler.get_shader_resources();

		for (const auto &image : shaderResources.sampled_images)
		{
			uint32_t set	 = compiler.get_decoration(image.id, spv::DecorationDescriptorSet);
			uint32_t binding = compiler.get_decoration(image.id, spv::DecorationBinding);

			ResourceBinding resource;
			resource.Name	 = image.name;
			resource.Set	 = set;
			resource.Binding = binding;
			resources.SampledImages.push_back(resource);
		}

		for (const auto &uniformBuffer : shaderResources.uniform_buffers)
		{
			uint32_t set	 = compiler.get_decoration(uniformBuffer.id, spv::DecorationDescriptorSet);
			uint32_t binding = compiler.get_decoration(uniformBuffer.id, spv::DecorationBinding);

			ResourceBinding resource;
			resource.Name	 = uniformBuffer.name;
			resource.Set	 = set;
			resource.Binding = binding;
			resources.UniformBuffers.push_back(resource);
		}
	}

	ShaderDataType GetShaderDataTypeFromSpirvCrossType(const spirv_cross::Compiler &compiler, const spirv_cross::Resource &resource)
	{
		const auto &baseType = compiler.get_type(resource.base_type_id);
		const auto &type	 = compiler.get_type(resource.type_id);

		switch (type.basetype)
		{
			case spirv_cross::SPIRType::BaseType::SByte:
			{
				switch (type.vecsize)
				{
					case 1: return ShaderDataType::R8_SInt;
					case 2: return ShaderDataType::R8G8_SInt;
					case 4: return ShaderDataType::R8G8B8A8_SInt;
					default: throw std::runtime_error("Invalid variable count");
				}
			}
			case spirv_cross::SPIRType::BaseType::UByte:
			{
				switch (type.vecsize)
				{
					case 1: return ShaderDataType::R8_UInt;
					case 2: return ShaderDataType::R8G8_UInt;
					case 4: return ShaderDataType::R8G8B8A8_UInt;
					default: throw std::runtime_error("Invalid variable count");
				}
			}
			case spirv_cross::SPIRType::BaseType::Short:
			{
				switch (type.vecsize)
				{
					case 1: return ShaderDataType::R16_SInt;
					case 2: return ShaderDataType::R16G16_SInt;
					case 4: return ShaderDataType::R16G16B16A16_SInt;
					default: throw std::runtime_error("Invalid variable count");
				}
			}
			case spirv_cross::SPIRType::BaseType::UShort:
			{
				switch (type.vecsize)
				{
					case 1: return ShaderDataType::R16_UInt;
					case 2: return ShaderDataType::R16G16_UInt;
					case 4: return ShaderDataType::R16G16B16A16_UInt;
					default: throw std::runtime_error("Invalid variable count");
				}
			}
			case spirv_cross::SPIRType::BaseType::Int:
			{
				switch (type.vecsize)
				{
					case 1: return ShaderDataType::R32_SInt;
					case 2: return ShaderDataType::R32G32_SInt;
					case 3: return ShaderDataType::R32G32B32_SInt;
					case 4: return ShaderDataType::R32G32B32A32_SInt;
					default: throw std::runtime_error("Invalid variable count");
				}
			}
			case spirv_cross::SPIRType::BaseType::UInt:
			{
				switch (type.vecsize)
				{
					case 1: return ShaderDataType::R16_UInt;
					case 2: return ShaderDataType::R16G16_UInt;
					case 4: return ShaderDataType::R16G16B16A16_UInt;
					default: throw std::runtime_error("Invalid variable count");
				}
			}
			case spirv_cross::SPIRType::BaseType::Half:
			{
				switch (type.vecsize)
				{
					case 1: return ShaderDataType::R16_SFloat;
					case 2: return ShaderDataType::R16G16_SFloat;
					case 4: return ShaderDataType::R16G16B16A16_SFloat;
					default: throw std::runtime_error("Invalid variable count");
				}
			}
			case spirv_cross::SPIRType::BaseType::Float:
			{
				switch (type.vecsize)
				{
					case 1: return ShaderDataType::R32_SFloat;
					case 2: return ShaderDataType::R32G32_SFloat;
					case 3: return ShaderDataType::R32G32B32_SFloat;
					case 4: return ShaderDataType::R32G32B32A32_SFloat;
					default: throw std::runtime_error("Invalid variable count");
				}
			}
			default: throw std::runtime_error("Failed to find a valid shader data type");
		}
	}

	void GetShaderInfo(const spirv_cross::Compiler &compiler, std::vector<ShaderAttribute> &inputs, std::vector<ShaderAttribute> &outputs)
	{
		const auto &resources = compiler.get_shader_resources();

		for (const auto &input : resources.stage_inputs)
		{
			const auto &baseType = compiler.get_type(input.base_type_id);
			const auto &type	 = compiler.get_type(input.type_id);

			ShaderAttribute attribute;
			attribute.Name = input.name;
			attribute.DataType = GetShaderDataTypeFromSpirvCrossType(compiler, input);
			inputs.push_back(attribute);
		}

		for (const auto &output : resources.stage_outputs)
		{
			const auto &baseType = compiler.get_type(output.base_type_id);
			const auto &type	 = compiler.get_type(output.type_id);

			ShaderAttribute attribute;
			attribute.Name = output.name;
			attribute.DataType = GetShaderDataTypeFromSpirvCrossType(compiler, output);
			outputs.push_back(attribute);
		}
	}

	CompilationResult ShaderGenerator::Generate(const std::string &source, ShaderGenerationOptions options, ResourceSetSpecification &resources)
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
		compileOptions.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
		compileOptions.SetTargetSpirv(shaderc_spirv_version_1_6);
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

		// compile to shader language
		switch (options.OutputFormat)
		{
			case ShaderLanguage::GLSL:
			{
				spirv_cross::CompilerGLSL compiler(spirv_binary);
				glOptions.version = 450;
				glOptions.es	  = false;
				compiler.set_common_options(glOptions);
				GetShaderInfo(compiler, output.InputAttributes, output.OutputAttributes);
				CreateResourceSetSpecification(compiler, resources);
				ToLinearResourceSet(compiler, options.OutputFormat);
				output.Source = compiler.compile();
				break;
			}
			case ShaderLanguage::GLSLES:
			{
				spirv_cross::CompilerGLSL compiler(spirv_binary);
				glOptions.version = 300;
				glOptions.es	  = true;
				compiler.set_common_options(glOptions);
				GetShaderInfo(compiler, output.InputAttributes, output.OutputAttributes);
				CreateResourceSetSpecification(compiler, resources);
				ToLinearResourceSet(compiler, options.OutputFormat);
				output.Source = compiler.compile();
				break;
			}
			case ShaderLanguage::HLSL:
			{
				spirv_cross::CompilerHLSL compiler(spirv_binary);

				const std::string name = GetD3DShaderEntryPoint(options.Stage);
				compiler.rename_entry_point("main", name.c_str(), GetShaderExecutionModel(options.Stage));

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
				GetShaderInfo(compiler, output.InputAttributes, output.OutputAttributes);
				CreateResourceSetSpecification(compiler, resources);
				ToLinearResourceSet(compiler, options.OutputFormat);
				output.Source = compiler.compile();
				break;
			}
			case ShaderLanguage::SPIRV:
			{
				spirv_cross::CompilerGLSL compiler(spirv_binary);
				glOptions.version = 450;
				glOptions.es	  = false;
				compiler.set_common_options(glOptions);
				GetShaderInfo(compiler, output.InputAttributes, output.OutputAttributes);
				CreateResourceSetSpecification(compiler, resources);
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
			case Nexus::Graphics::ShaderStage::TesselationControl: return "tcs_main";
			case Nexus::Graphics::ShaderStage::TesselationEvaluation: return "tes_main";
			case Nexus::Graphics::ShaderStage::Vertex: return "vs_main";
			case Nexus::Graphics::ShaderStage::Mesh: return "ms_main";
			case Nexus::Graphics::ShaderStage::Task: return "ts_main";
			default: throw std::runtime_error("Failed to find a valid shader stage");
		}
	}
}	 // namespace Nexus::Graphics