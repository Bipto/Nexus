#pragma once

#include <string>
#include <inttypes.h>

namespace Nexus
{
    enum class ShaderType
    {
        None = -1,
        Vertex = 0,
        Fragment = 1
    };

    enum class ShaderFormat
    {
        GLSL,
        GLSLES,
        HLSL,
        SPIRV
    };

#ifndef __EMSCRIPTEN__

    struct ShaderGenerationOptions
    {
        ShaderType Type;
        ShaderFormat OutputFormat;
        std::string ShaderName;
    };

    struct CompilationResult
    {
        bool Successful;
        std::string Source;
        std::vector<uint32_t> SpirvBuffer;
        std::string Error;
        ShaderFormat OutputFormat;
    };

    class ShaderGenerator
    {
    public:
        CompilationResult Generate(std::string source, ShaderGenerationOptions options);
    };

#endif
}