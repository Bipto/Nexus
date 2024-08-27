#include "GraphicsDevice.hpp"

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Logging/Log.hpp"
#include "ShaderGenerator.hpp"
#include "ShaderUtils.hpp"

#include "Nexus-Core/FileSystem/FileSystem.hpp"

#include "Nexus-Core/Graphics/MipmapGenerator.hpp"

#include "stb_image.h"

namespace Nexus::Graphics
{
GraphicsDevice::GraphicsDevice(const GraphicsDeviceSpecification &specification, Window *window, const SwapchainSpecification &swapchainSpec)
{
    m_Window = window;
    m_Specification = specification;
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

void GraphicsDevice::ImmediateSubmit(std::function<void(Ref<CommandList>)> &&function)
{
    if (!m_ImmediateCommandList)
    {
        m_ImmediateCommandList = CreateCommandList();
    }

    m_ImmediateCommandList->Begin();
    function(m_ImmediateCommandList);
    m_ImmediateCommandList->End();
    SubmitCommandList(m_ImmediateCommandList);
}

const GraphicsDeviceSpecification &GraphicsDevice::GetSpecification() const
{
    return m_Specification;
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
    texture->SetData(data, 0, 0, 0, spec.Width, spec.Height);
    // texture->SetData(data, spec.Width * spec.Height * sizeof(unsigned char), 0);

    if (generateMips)
    {
        uint32_t mipsToGenerate = spec.Levels - 1;
        Nexus::Graphics::MipmapGenerator mipGenerator(this);
        mipGenerator.GenerateMips(texture, mipsToGenerate);
    }

    stbi_image_free(data);
    return texture;
}

Ref<Texture> GraphicsDevice::CreateTexture(const std::string &filepath, bool generateMips)
{
    return CreateTexture(filepath.c_str(), generateMips);
}

Ref<ResourceSet> GraphicsDevice::CreateResourceSet(Ref<Pipeline> pipeline)
{
    return CreateResourceSet(pipeline->GetPipelineDescription().ResourceSetSpec);
}
} // namespace Nexus::Graphics