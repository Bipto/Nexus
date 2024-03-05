#include "ShaderGenerator.hpp"

#include "shaderc/shaderc.hpp"
#include "spirv_glsl.hpp"
#include "spirv_hlsl.hpp"

#include "Nexus/Logging/Log.hpp"

#include "ResourceSet.hpp"

namespace Nexus::Graphics
{
    shaderc_shader_kind GetTypeOfShader(ShaderType type)
    {
        switch (type)
        {
        case ShaderType::Vertex:
            return shaderc_glsl_vertex_shader;
        case ShaderType::Fragment:
            return shaderc_glsl_fragment_shader;
        }
    }

    void ToLinearResourceSet(spirv_cross::Compiler &compiler, ShaderLanguage language)
    {
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        ResourceSetSpecification resourceSpec;

        // find all resources in shaders
        for (const auto &image : resources.sampled_images)
        {
            uint32_t set = compiler.get_decoration(image.id, spv::DecorationDescriptorSet);
            uint32_t binding = compiler.get_decoration(image.id, spv::DecorationBinding);

            ResourceBinding resource;
            resource.Name = image.name;
            resource.Set = set;
            resource.Binding = binding;
            resource.Type = ResourceType::CombinedImageSampler;
            resourceSpec.Textures.push_back(resource);

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

            ResourceBinding resource;
            resource.Name = uniformBuffer.name;
            resource.Set = set;
            resource.Binding = binding;
            resource.Type = ResourceType::UniformBuffer;
            resourceSpec.UniformBuffers.push_back(resource);

            uint32_t slot = (set * ResourceSet::DescriptorSetCount) + binding;
            compiler.unset_decoration(uniformBuffer.id, spv::DecorationDescriptorSet);

            if (language == ShaderLanguage::GLSL | language == ShaderLanguage::GLSLES)
            {
                compiler.unset_decoration(uniformBuffer.id, spv::DecorationBinding);
            }
            else
            {
                compiler.set_decoration(uniformBuffer.id, spv::DecorationBinding, slot);
            }
        }
    }

    CompilationResult ShaderGenerator::Generate(const std::string &source, ShaderGenerationOptions options)
    {
        CompilationResult output;
        output.Successful = false;
        output.Source = {};
        output.Error = {};
        output.OutputFormat = options.OutputFormat;

        // compile to SPIR-V
        shaderc::Compiler compiler;
        auto shaderType = GetTypeOfShader(options.Type);
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
            spirv_cross::CompilerGLSL glsl(spirv_binary);
            glOptions.version = 330;
            glOptions.es = false;
            glsl.set_common_options(glOptions);
            ToLinearResourceSet(glsl, options.OutputFormat);
            output.Source = glsl.compile();
            break;
        }
        case ShaderLanguage::GLSLES:
        {
            spirv_cross::CompilerGLSL glsl(spirv_binary);
            glOptions.version = 300;
            glOptions.es = true;
            glsl.set_common_options(glOptions);
            ToLinearResourceSet(glsl, options.OutputFormat);
            output.Source = glsl.compile();
            break;
        }
        case ShaderLanguage::HLSL:
        {
            spirv_cross::CompilerHLSL hlsl(spirv_binary);

            if (options.Type == ShaderType::Vertex)
            {
                hlsl.rename_entry_point("main", "vs_main", spv::ExecutionModel::ExecutionModelVertex);
            }
            else if (options.Type == ShaderType::Fragment)
            {
                hlsl.rename_entry_point("main", "ps_main", spv::ExecutionModel::ExecutionModelFragment);
            }
            else
            {
                NX_ERROR("Unsupported shader type");
            }

            glOptions.version = 330;
            glOptions.es = false;
            hlsl.set_common_options(glOptions);

            spirv_cross::CompilerHLSL::Options hlslOptions;
            // allow the main method to be renamed
            hlslOptions.use_entry_point_name = true;
            // modern HLSL
            hlslOptions.shader_model = 50;
            hlsl.set_hlsl_options(hlslOptions);
            ToLinearResourceSet(hlsl, options.OutputFormat);
            output.Source = hlsl.compile();
            break;
        }
        case ShaderLanguage::SPIRV:
            output.Source = source;
            break;
        default:
            throw std::runtime_error("Failed to find a valid shader format");
        }

        output.Successful = true;
        return output;
    }
}