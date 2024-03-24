#include "ShaderGenerator.hpp"

#include "shaderc/shaderc.hpp"
#include "spirv_glsl.hpp"
#include "spirv_hlsl.hpp"

#include "Nexus/Logging/Log.hpp"

#include "Platform/D3D12/D3D12Utils.hpp"

#include "ResourceSet.hpp"

spv::ExecutionModel GetShaderExecutionModel(Nexus::Graphics::ShaderStage stage)
{
    switch (stage)
    {
    case Nexus::Graphics::ShaderStage::Compute:
        return spv::ExecutionModel::ExecutionModelGLCompute;
    case Nexus::Graphics::ShaderStage::Fragment:
        return spv::ExecutionModel::ExecutionModelFragment;
    case Nexus::Graphics::ShaderStage::Geometry:
        return spv::ExecutionModel::ExecutionModelGeometry;
    case Nexus::Graphics::ShaderStage::TesselationControl:
        return spv::ExecutionModel::ExecutionModelTessellationControl;
    case Nexus::Graphics::ShaderStage::TesselationEvaluation:
        return spv::ExecutionModel::ExecutionModelTessellationEvaluation;
    case Nexus::Graphics::ShaderStage::Vertex:
        return spv::ExecutionModel::ExecutionModelVertex;
    default:
        throw std::runtime_error("Failed to find a valid shader stage");
    }
}

namespace Nexus::Graphics
{
    shaderc_shader_kind GetTypeOfShader(ShaderStage stage)
    {
        switch (stage)
        {
        case ShaderStage::Compute:
            return shaderc_glsl_compute_shader;
        case ShaderStage::Fragment:
            return shaderc_glsl_fragment_shader;
        case ShaderStage::Geometry:
            return shaderc_glsl_geometry_shader;
        case ShaderStage::TesselationControl:
            return shaderc_glsl_tess_control_shader;
        case ShaderStage::TesselationEvaluation:
            return shaderc_glsl_tess_evaluation_shader;
        case ShaderStage::Vertex:
            return shaderc_glsl_vertex_shader;
        default:
            throw std::runtime_error("Failed to find a valid shader stage");
        }
    }

    void ToLinearResourceSet(spirv_cross::Compiler &compiler, ShaderLanguage language)
    {
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        // find all resources in shaders
        for (const auto &image : resources.sampled_images)
        {
            uint32_t set = compiler.get_decoration(image.id, spv::DecorationDescriptorSet);
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
            uint32_t set = compiler.get_decoration(uniformBuffer.id, spv::DecorationDescriptorSet);
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
            uint32_t set = compiler.get_decoration(image.id, spv::DecorationDescriptorSet);
            uint32_t binding = compiler.get_decoration(image.id, spv::DecorationBinding);

            ResourceBinding resource;
            resource.Name = image.name;
            resource.Set = set;
            resource.Binding = binding;
            resource.Type = ResourceType::CombinedImageSampler;
            resources.SampledImages.push_back(resource);
        }

        for (const auto &uniformBuffer : shaderResources.uniform_buffers)
        {
            uint32_t set = compiler.get_decoration(uniformBuffer.id, spv::DecorationDescriptorSet);
            uint32_t binding = compiler.get_decoration(uniformBuffer.id, spv::DecorationBinding);

            ResourceBinding resource;
            resource.Name = uniformBuffer.name;
            resource.Set = set;
            resource.Binding = binding;
            resource.Type = ResourceType::UniformBuffer;
            resources.UniformBuffers.push_back(resource);
        }
    }

    CompilationResult ShaderGenerator::Generate(const std::string &source, ShaderGenerationOptions options, ResourceSetSpecification &resources)
    {
        CompilationResult output;
        output.Successful = false;
        output.Source = {};
        output.Error = {};
        output.OutputFormat = options.OutputFormat;

        // compile to SPIR-V
        shaderc::Compiler compiler;
        auto shaderType = GetTypeOfShader(options.Stage);
        shaderc::CompilationResult result = compiler.CompileGlslToSpv(source, shaderType, options.ShaderName.c_str());

        if (result.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            output.Error += result.GetErrorMessage();
            return output;
        }

        std::vector<uint32_t> spirv_binary = {result.begin(), result.end()};
        output.SpirvBinary = spirv_binary;

        spirv_cross::CompilerGLSL::Options glOptions;
        glOptions.emit_push_constant_as_uniform_buffer = true;

        // compile to shader language
        switch (options.OutputFormat)
        {
        case ShaderLanguage::GLSL:
        {
            spirv_cross::CompilerGLSL compiler(spirv_binary);
            glOptions.version = 450;
            glOptions.es = false;
            compiler.set_common_options(glOptions);
            CreateResourceSetSpecification(compiler, resources);
            ToLinearResourceSet(compiler, options.OutputFormat);
            output.Source = compiler.compile();
            break;
        }
        case ShaderLanguage::GLSLES:
        {
            spirv_cross::CompilerGLSL compiler(spirv_binary);
            glOptions.version = 300;
            glOptions.es = true;
            compiler.set_common_options(glOptions);
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
            glOptions.es = false;
            compiler.set_common_options(glOptions);

            spirv_cross::CompilerHLSL::Options hlslOptions;
            // allow the main method to be renamed
            hlslOptions.use_entry_point_name = true;
            // modern HLSL
            hlslOptions.shader_model = 50;
            hlslOptions.flatten_matrix_vertex_input_semantics = true;
            compiler.set_hlsl_options(hlslOptions);
            CreateResourceSetSpecification(compiler, resources);
            ToLinearResourceSet(compiler, options.OutputFormat);
            output.Source = compiler.compile();
            break;
        }
        case ShaderLanguage::SPIRV:
        {
            spirv_cross::CompilerGLSL compiler(spirv_binary);
            glOptions.version = 450;
            glOptions.es = false;
            compiler.set_common_options(glOptions);
            CreateResourceSetSpecification(compiler, resources);
            output.Source = source;
            break;
        }

        default:
            throw std::runtime_error("Failed to find a valid shader format");
        }

        if (options.OutputFormat == ShaderLanguage::SPIRV)
        {
            shaderc::Compiler compiler;
            auto shaderType = GetTypeOfShader(options.Stage);
            shaderc::CompilationResult result = compiler.CompileGlslToSpv(output.Source, shaderType, options.ShaderName.c_str());

            std::vector<uint32_t> spirv_binary = {result.begin(), result.end()};
            output.SpirvBinary = spirv_binary;
        }

        output.Successful = true;
        return output;
    }

    std::string GetD3DShaderEntryPoint(Nexus::Graphics::ShaderStage stage)
    {
        switch (stage)
        {
        case Nexus::Graphics::ShaderStage::Compute:
            return "cs_main";
        case Nexus::Graphics::ShaderStage::Fragment:
            return "fs_main";
        case Nexus::Graphics::ShaderStage::Geometry:
            return "gs_main";
        case Nexus::Graphics::ShaderStage::TesselationControl:
            return "tcs_main";
        case Nexus::Graphics::ShaderStage::TesselationEvaluation:
            return "tes_main";
        case Nexus::Graphics::ShaderStage::Vertex:
            return "vs_main";
        default:
            throw std::runtime_error("Failed to find a valid shader stage");
        }
    }
}