#include "GraphicsDevice.hpp"

#include <fstream>
#include <sstream>
#include <chrono>

#include "Nexus/Logging/Log.hpp"
#include "ShaderGenerator.hpp"
#include "ShaderUtils.hpp"

#include "Nexus/FileSystem/FileSystem.hpp"

#include "Nexus/Graphics/MipmapGenerator.hpp"

#include "stb_image.h"

namespace Nexus::Graphics
{
    GraphicsDevice::GraphicsDevice(const GraphicsDeviceCreateInfo &createInfo, Window *window, const SwapchainSpecification &swapchainSpec)
    {
        m_Window = window;
        m_API = createInfo.API;
    }

    Ref<ShaderModule> GraphicsDevice::CreateShaderModuleFromSpirvFile(const std::string &filepath, ShaderStage stage)
    {
        std::string shaderSource = Nexus::FileSystem::ReadFileToString(filepath);
        return CreateShaderModuleFromSpirvSource(shaderSource, filepath, stage);
    }

    Ref<ShaderModule> GraphicsDevice::CreateShaderModuleFromSpirvSource(const std::string &source, const std::string &name, ShaderStage stage)
    {
        ShaderModuleSpecification moduleSpec;
        ResourceSetSpecification resourceSetSpec;

        auto startTime = std::chrono::system_clock::now();

        ShaderGenerator generator;
        std::string errorMessage;

        ShaderGenerationOptions options;
        options.Stage = stage;
        options.ShaderName = name;
        options.OutputFormat = GetSupportedShaderFormat();

        auto result = generator.Generate(source, options, resourceSetSpec);

        if (!result.Successful)
        {
            throw std::runtime_error(result.Error);
        }

        auto endTime = std::chrono::system_clock::now();
        auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

        moduleSpec.Name = name;
        moduleSpec.Source = result.Source;
        moduleSpec.Stage = stage;
        moduleSpec.SpirvBinary = result.SpirvBinary;
        moduleSpec.InputAttributes = result.InputAttributes;
        moduleSpec.OutputAttributes = result.OutputAttributes;

        return CreateShaderModule(moduleSpec, resourceSetSpec);
    }

    Window *GraphicsDevice::GetPrimaryWindow()
    {
        return m_Window;
    }

    Ref<Texture> GraphicsDevice::CreateTexture(const char *filepath, bool generateMips)
    {
        int desiredChannels = 4;
        int receivedChannels = 0;
        int width = 0;
        int height = 0;

        TextureSpecification spec;
        spec.Format = PixelFormat::R8_G8_B8_A8_UNorm;
        unsigned char *data = stbi_load(filepath, &width, &height, &receivedChannels, desiredChannels);
        spec.Width = (uint32_t)width;
        spec.Height = (uint32_t)height;

        if (generateMips)
        {
            uint32_t mipCount = Nexus::Graphics::MipmapGenerator::GetMaximumNumberOfMips(spec.Width, spec.Height);
            spec.Levels = mipCount;
        }
        else
        {
            spec.Levels = 1;
        }

        auto texture = CreateTexture(spec);
        // texture->SetData(data, 0, 0, 0, spec.Width, spec.Height);
        texture->SetData(data, spec.Width * spec.Height * sizeof(unsigned char), 0);

        stbi_image_free(data);
        return texture;
    }

    Ref<Texture> GraphicsDevice::CreateTexture(const std::string &filepath, bool generateMips)
    {
        return CreateTexture(filepath.c_str());
    }

    Ref<ResourceSet> GraphicsDevice::CreateResourceSet(Ref<Pipeline> pipeline)
    {
        return CreateResourceSet(pipeline->GetPipelineDescription().ResourceSetSpecification);
    }
}