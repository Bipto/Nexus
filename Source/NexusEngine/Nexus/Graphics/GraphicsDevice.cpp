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
    GraphicsDevice::GraphicsDevice(const GraphicsDeviceCreateInfo &createInfo, Window *window)
    {
        m_Window = window;
        m_API = createInfo.API;
    }

    Shader *GraphicsDevice::CreateShaderFromSpirvFile(const std::string &filepath, const VertexBufferLayout &layout)
    {
        auto startTime = std::chrono::system_clock::now();
        Nexus::Utils::ShaderSources sources = Nexus::Utils::ParseCustomShaderFile(filepath);

        // if we are using a SPIR_V bytecode we do not need to compile the GLSL to a native shader language
        if (this->GetSupportedShaderFormat() != Nexus::Graphics::ShaderLanguage::SPIRV)
        {
            ShaderGenerator generator;
            std::string errorMessage;

            ShaderGenerationOptions vertOptions;
            vertOptions.Type = ShaderType::Vertex;
            vertOptions.ShaderName = filepath;
            vertOptions.OutputFormat = this->GetSupportedShaderFormat();
            auto vertResult = generator.Generate(sources.VertexSource, vertOptions);

            if (!vertResult.Successful)
            {
                errorMessage += "Error compiling vertex shader: " + vertResult.Error + "\n";
            }

            ShaderGenerationOptions fragOptions;
            fragOptions.Type = ShaderType::Fragment;
            fragOptions.ShaderName = filepath;
            fragOptions.OutputFormat = this->GetSupportedShaderFormat();
            auto fragResult = generator.Generate(sources.FragmentSource, fragOptions);

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
            auto shader = this->CreateShaderFromSource(sources.VertexSource, sources.FragmentSource, layout);
            return shader;
        }

        return {};
    }

    Texture *GraphicsDevice::CreateTexture(const char *filepath)
    {
        int desiredChannels = 4;

        TextureSpecification spec;
        spec.Format = TextureFormat::RGBA8;
        spec.Data = stbi_load(filepath, &spec.Width, &spec.Height, &spec.NumberOfChannels, desiredChannels);
        spec.NumberOfChannels = desiredChannels;

        auto texture = CreateTexture(spec);
        delete[] spec.Data;
        return texture;
    }

    std::pair<Framebuffer *, RenderPass *> GraphicsDevice::CreateRenderPassAndFramebuffer(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &framebufferSpecification)
    {
        auto container = std::pair<Framebuffer *, RenderPass *>();
        container.second = CreateRenderPass(renderPassSpecification, framebufferSpecification);
        container.first = CreateFramebuffer(container.second);
        return container;
    }

    ResourceSet *GraphicsDevice::CreateResourceSet(Pipeline *pipeline)
    {
        return CreateResourceSet(pipeline->GetPipelineDescription().ResourceSetSpecification);
    }
}