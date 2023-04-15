#include "GraphicsDevice.h"

#include "shaderc/shaderc.hpp"

#include <fstream>
#include <sstream>
#include <chrono>

#include "ShaderGenerator.h"
#include "Core/Logging/Log.h"

namespace Nexus
{
    Ref<Shader> GraphicsDevice::CreateShaderFromSpirvFile(const std::string &filepath, const BufferLayout& layout)
    {
        auto startTime = std::chrono::system_clock::now();

        if (!std::filesystem::exists(filepath))
        {
            std::string message = "File: " + filepath + " does not exist!";
            NX_ERROR(message);
            return {};
        }

        std::ifstream stream(filepath);

        struct ShaderSources
        {
            std::string VertexSource;
            std::string FragmentSource;
        };

        std::string line;
        std::stringstream ss[2];
        ShaderType type = ShaderType::None;

        while (getline(stream, line))
        {
            if (line.find("#shader") != std::string::npos)
            {
                if (line.find("vertex") != std::string::npos)
                    type = ShaderType::Vertex;
                else if (line.find("fragment") != std::string::npos)
                    type = ShaderType::Fragment;
            }
            else
            {
                ss[(int)type] << line << "\n";
            }
        }

        ShaderSources source;
        source.VertexSource = ss[0].str();
        source.FragmentSource = ss[1].str();

        Nexus::ShaderGenerator generator;
        std::string errorMessage;

        Nexus::ShaderGenerationOptions vertOptions;
        vertOptions.Type = ShaderType::Vertex;
        vertOptions.ShaderName = filepath;
        vertOptions.OutputFormat = this->GetSupportedShaderFormat();
        auto vertResult = generator.Generate(source.VertexSource, vertOptions);

        if (!vertResult.Successful)
        {
            errorMessage += "Error compiling vertex shader: " + vertResult.Error + "\n";
        }

        Nexus::ShaderGenerationOptions fragOptions;
        fragOptions.Type = ShaderType::Fragment;
        fragOptions.ShaderName = filepath;
        fragOptions.OutputFormat = this->GetSupportedShaderFormat();
        auto fragResult = generator.Generate(source.FragmentSource, fragOptions);

        if (!fragResult.Successful)
        {
            errorMessage += "Error compiling fragment shader: " + fragResult.Error;
        }

        auto endTime = std::chrono::system_clock::now();
        auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

        if (vertResult.Successful && fragResult.Successful)
        {
            std::stringstream ss;
            ss << "Compilation of " << filepath << " took " << totalTime << " milliseconds";
            NX_LOG(ss.str());

            auto shader = this->CreateShaderFromSource(vertResult.Source, fragResult.Source, layout);
            return shader;
        }   
        else
        {
            NX_ERROR(errorMessage);
        }     

        return {};
    }
}