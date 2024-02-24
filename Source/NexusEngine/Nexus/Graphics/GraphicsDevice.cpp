#include "GraphicsDevice.hpp"

#include <fstream>
#include <sstream>
#include <chrono>

#include "Nexus/Logging/Log.hpp"
#include "ShaderGenerator.hpp"
#include "ShaderUtils.hpp"

#include "stb_image.h"

namespace Nexus::Graphics
{
    GraphicsDevice::GraphicsDevice(const GraphicsDeviceCreateInfo &createInfo, Window *window, const SwapchainSpecification &swapchainSpec)
    {
        m_Window = window;
        m_API = createInfo.API;
    }

    Shader *GraphicsDevice::CreateShaderFromSpirvFile(const std::string &filepath, const VertexBufferLayout &layout)
    {
        std::cout << "Creating shader: " << filepath << "\n";
        Nexus::Utils::ShaderSources sources = Nexus::Utils::ParseCustomShaderFile(filepath);
        auto shader = CreateShaderFromSpirvSources(sources.VertexSource, sources.FragmentSource, layout, filepath, filepath);
        return shader;
    }

    Shader *GraphicsDevice::CreateShaderFromSpirvSources(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const VertexBufferLayout &layout, const std::string &vertexShaderName, const std::string &fragmentShaderName)
    {
        auto startTime = std::chrono::system_clock::now();

        // if we are using a SPIR_V bytecode we do not need to compile the GLSL to a native shader language
        if (this->GetSupportedShaderFormat() != Nexus::Graphics::ShaderLanguage::SPIRV)
        {
            ShaderGenerator generator;
            std::string errorMessage;

            ShaderGenerationOptions vertOptions;
            vertOptions.Type = ShaderType::Vertex;
            vertOptions.ShaderName = vertexShaderName;
            vertOptions.OutputFormat = this->GetSupportedShaderFormat();
            auto vertResult = generator.Generate(vertexShaderSource, vertOptions);

            if (!vertResult.Successful)
            {
                errorMessage += "Error compiling vertex shader: " + vertResult.Error + "\n";
            }

            ShaderGenerationOptions fragOptions;
            fragOptions.Type = ShaderType::Fragment;
            fragOptions.ShaderName = fragmentShaderName;
            fragOptions.OutputFormat = this->GetSupportedShaderFormat();
            auto fragResult = generator.Generate(fragmentShaderSource, fragOptions);

            if (!fragResult.Successful)
            {
                errorMessage += "Error compiling fragment shader: " + fragResult.Error;
            }

            auto endTime = std::chrono::system_clock::now();
            auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

            if (vertResult.Successful && fragResult.Successful)
            {
                std::stringstream ss;
                ss << "Compilation of " << vertexShaderName << " and " << fragmentShaderName << " took " << totalTime << " milliseconds";

                auto shader = this->CreateShaderFromSource(vertResult.Source, fragResult.Source, layout);
                return shader;
            }
            else
            {
                NX_ERROR(errorMessage);
            }
        }

        else
        {
            auto shader = this->CreateShaderFromSource(vertexShaderSource, fragmentShaderSource, layout);
            return shader;
        }
    }

    Window *GraphicsDevice::GetPrimaryWindow()
    {
        return m_Window;
    }

    Texture *GraphicsDevice::CreateTexture(const char *filepath)
    {
        int desiredChannels = 4;

        TextureSpecification spec;
        spec.Format = PixelFormat::R8_G8_B8_A8_UNorm;
        unsigned char *data = stbi_load(filepath, &spec.Width, &spec.Height, &spec.NumberOfChannels, desiredChannels);
        spec.NumberOfChannels = desiredChannels;

        auto texture = CreateTexture(spec);
        texture->SetData(data, spec.Width * spec.Height * spec.NumberOfChannels * sizeof(unsigned char));
        delete[] data;
        return texture;
    }

    Texture *GraphicsDevice::CreateTexture(const std::string &filepath)
    {
        return CreateTexture(filepath.c_str());
    }

    ResourceSet *GraphicsDevice::CreateResourceSet(Pipeline *pipeline)
    {
        return CreateResourceSet(pipeline->GetPipelineDescription().ResourceSetSpecification);
    }
}