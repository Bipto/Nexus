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

    void ToLinearResourceSet(spirv_cross::Compiler &compiler)
    {
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        ResourceSetSpecification resourceSpec;

        // find all resources in shaders
        for (const auto &image : resources.sampled_images)
        {
            uint32_t set = compiler.get_decoration(image.id, spv::DecorationDescriptorSet);
            uint32_t binding = compiler.get_decoration(image.id, spv::DecorationBinding);

            /* uint32_t newBinding = ResourceSet::GetLinearDescriptorSlot(set, binding);
            compiler.unset_decoration(image.id, spv::DecorationDescriptorSet);
            compiler.set_decoration(image.id, spv::DecorationBinding, newBinding); */

            Nexus::Graphics::ResourceBinding resBinding;
            resBinding.Name = image.name;
            resBinding.Set = set;
            resBinding.Binding = binding;
            resBinding.Type = Nexus::Graphics::ResourceType::CombinedImageSampler;
            resourceSpec.Textures.push_back(resBinding);
        }

        for (const auto &uniformBuffer : resources.uniform_buffers)
        {
            uint32_t set = compiler.get_decoration(uniformBuffer.id, spv::DecorationDescriptorSet);
            uint32_t binding = compiler.get_decoration(uniformBuffer.id, spv::DecorationBinding);

            /* uint32_t newBinding = ResourceSet::GetLinearDescriptorSlot(set, binding);
            compiler.unset_decoration(uniformBuffer.id, spv::DecorationDescriptorSet);
            compiler.set_decoration(uniformBuffer.id, spv::DecorationBinding, newBinding); */

            Nexus::Graphics::ResourceBinding resBinding;
            resBinding.Name = uniformBuffer.name;
            resBinding.Set = set;
            resBinding.Binding = binding;
            resBinding.Type = Nexus::Graphics::ResourceType::UniformBuffer;
            resourceSpec.UniformBuffers.push_back(resBinding);
        }

        const auto &remappedTextureBindings = ResourceSet::RemapToLinearBindings(resourceSpec.Textures);
        const auto &remappedUniformBufferBindings = ResourceSet::RemapToLinearBindings(resourceSpec.UniformBuffers);

        for (int i = 0; i < resources.sampled_images.size(); i++)
        {
            auto &image = resources.sampled_images[i];
            uint32_t set = compiler.get_decoration(image.id, spv::DecorationDescriptorSet);
            uint32_t binding = compiler.get_decoration(image.id, spv::DecorationBinding);

            uint32_t index = ResourceSet::GetLinearDescriptorSlot(set, binding);
            uint32_t newBinding = remappedTextureBindings.at(index);
            compiler.unset_decoration(image.id, spv::DecorationDescriptorSet);
            compiler.set_decoration(image.id, spv::DecorationBinding, newBinding);
        }

        for (int i = 0; i < resources.uniform_buffers.size(); i++)
        {
            auto &uniformBuffer = resources.uniform_buffers[i];
            uint32_t set = compiler.get_decoration(uniformBuffer.id, spv::DecorationDescriptorSet);
            uint32_t binding = compiler.get_decoration(uniformBuffer.id, spv::DecorationBinding);

            uint32_t index = ResourceSet::GetLinearDescriptorSlot(set, binding);
            uint32_t newBinding = remappedUniformBufferBindings.at(index);
            compiler.unset_decoration(uniformBuffer.id, spv::DecorationDescriptorSet);
            compiler.set_decoration(uniformBuffer.id, spv::DecorationBinding, newBinding);
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

        // compile to shader language
        switch (options.OutputFormat)
        {
        case ShaderLanguage::GLSL:
        {
            spirv_cross::CompilerGLSL glsl(spirv_binary);
            spirv_cross::CompilerGLSL::Options glOptions;
            glOptions.version = 330;
            glOptions.es = false;
            glOptions.emit_push_constant_as_uniform_buffer = true;
            glsl.set_common_options(glOptions);
            ToLinearResourceSet(glsl);
            output.Source = glsl.compile();
            break;
        }
        case ShaderLanguage::GLSLES:
        {
            spirv_cross::CompilerGLSL glsl(spirv_binary);
            spirv_cross::CompilerGLSL::Options glOptions;
            glOptions.version = 300;
            glOptions.es = true;
            glOptions.emit_push_constant_as_uniform_buffer = true;
            glsl.set_common_options(glOptions);
            ToLinearResourceSet(glsl);
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

            spirv_cross::CompilerGLSL::Options glOptions;
            glOptions.version = 330;
            glOptions.es = false;
            glOptions.emit_push_constant_as_uniform_buffer = true;
            hlsl.set_common_options(glOptions);

            spirv_cross::CompilerHLSL::Options hlslOptions;
            // allow the main method to be renamed
            hlslOptions.use_entry_point_name = true;
            // modern HLSL
            hlslOptions.shader_model = 50;
            hlsl.set_hlsl_options(hlslOptions);
            ToLinearResourceSet(hlsl);
            output.Source = hlsl.compile();
            break;
        }
        case ShaderLanguage::SPIRV:
            output.Source = source;
            break;
        }

        output.Successful = true;

        std::cout << output.Source << "\n";

        return output;
    }
}