#include "ShaderGenerator.h"

#include "shaderc/shaderc.hpp"
#include "spirv_glsl.hpp"
#include "spirv_hlsl.hpp"

#include "Core/Logging/Log.h"

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

        // compile to shader language
        switch (options.OutputFormat)
        {
        case ShaderLanguage::GLSL:
        {
            spirv_cross::CompilerGLSL glsl(spirv_binary);
            spirv_cross::CompilerGLSL::Options glOptions;
            glOptions.version = 330;
            glOptions.es = false;
            glsl.set_common_options(glOptions);
            output.Source = glsl.compile();
            break;
        }
        case ShaderLanguage::GLSLES:
        {
            spirv_cross::CompilerGLSL glsl(spirv_binary);
            spirv_cross::CompilerGLSL::Options glOptions;
            glOptions.version = 300;
            glOptions.es = true;
            glsl.set_common_options(glOptions);
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
            hlsl.set_common_options(glOptions);

            spirv_cross::CompilerHLSL::Options hlslOptions;
            // allow the main method to be renamed
            hlslOptions.use_entry_point_name = true;
            // modern HLSL
            hlslOptions.shader_model = 50;
            hlsl.set_hlsl_options(hlslOptions);

            output.Source = hlsl.compile();
            break;
        }
        }

        std::cout << output.Source << std::endl;

        output.Successful = true;

        return output;
    }
}