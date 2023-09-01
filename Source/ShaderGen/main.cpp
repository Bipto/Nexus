#include <iostream>
#include <filesystem>

#include "Core/Graphics/ShaderGenerator.hpp"
#include "Core/Graphics/ShaderUtils.hpp"
#include "Core/FileSystem/FileSystem.hpp"

void ProcessFile(const std::string &filepath)
{
    std::cout << "Select an output shader language:\n";
    std::cout << "Enter 1 to generate GLSL, 2 to generate GLSLES or 3 to generate HLSL: ";

    int input = -1;
    std::cin >> input;

    Nexus::Graphics::ShaderLanguage language;

    switch (input)
    {
    case 1:
        language = Nexus::Graphics::ShaderLanguage::GLSL;
        break;
    case 2:
        language = Nexus::Graphics::ShaderLanguage::GLSLES;
        break;
    case 3:
        language = Nexus::Graphics::ShaderLanguage::HLSL;
        break;
    default:
        std::cout << "Invalid shader language selected\n";
        return;
        break;
    }

    std::filesystem::path filename = filepath;
    Nexus::Utils::ShaderSources sources = Nexus::Utils::ParseCustomShaderFile(filepath);

    Nexus::Graphics::ShaderGenerator generator;

    {
        Nexus::Graphics::ShaderGenerationOptions options;
        options.OutputFormat = language;
        options.Type = Nexus::Graphics::ShaderType::Vertex;
        const auto &result = generator.Generate(sources.VertexSource, options);
        std::string shaderName = filename.stem().string() + ".vertex.shader";
        Nexus::FileSystem::WriteFile(shaderName, result.Source);
    }

    {
        Nexus::Graphics::ShaderGenerationOptions options;
        options.OutputFormat = language;
        options.Type = Nexus::Graphics::ShaderType::Fragment;
        const auto &result = generator.Generate(sources.FragmentSource, options);
        std::string shaderName = filename.stem().string() + ".fragment.shader";
        Nexus::FileSystem::WriteFile(shaderName, result.Source);
    }
}

int main(int argc, char **argv)
{
    if (argc > 1)
    {
        std::string filepath = argv[1];
        ProcessFile(filepath);
    }

    return 0;
}