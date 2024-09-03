#include "HdriProcessor.hpp"

#include "Nexus-Core/Graphics/Framebuffer.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Graphics/MeshFactory.hpp"
#include "Nexus-Core/Graphics/Texture.hpp"

#include "stb_image.h"

namespace Nexus::Graphics
{
struct alignas(16) VB_UNIFORM_HDRI_PROCESSOR_CAMERA
{
    glm::mat4 View = {};
    glm::mat4 Projection = {};
};

HdriProcessor::HdriProcessor(const std::string &filepath, GraphicsDevice *device) : m_Device(device)
{
    m_Data = stbi_loadf(filepath.c_str(), &m_Width, &m_Height, &m_Channels, 4);

    Nexus::Graphics::Texture2DSpecification textureSpec{};
    textureSpec.Width = m_Width;
    textureSpec.Height = m_Height;
    textureSpec.Usage = {Nexus::Graphics::TextureUsage::Sampled};
    textureSpec.Format = Nexus::Graphics::PixelFormat::R32_G32_B32_A32_Float;
    m_HdriImage = m_Device->CreateTexture2D(textureSpec);

    m_HdriImage->SetData(m_Data, 0, 0, 0, m_Width, m_Height);
}

Ref<Cubemap> HdriProcessor::Generate(uint32_t size)
{
    Nexus::Graphics::FramebufferSpecification framebufferSpec = {};
    framebufferSpec.Width = size;
    framebufferSpec.Height = size;
    framebufferSpec.Samples = Nexus::Graphics::SampleCount::SampleCount1;
    framebufferSpec.ColorAttachmentSpecification = {PixelFormat::R32_G32_B32_A32_Float};
    framebufferSpec.DepthAttachmentSpecification = PixelFormat::D24_UNorm_S8_UInt;

    Ref<Framebuffer> framebuffer = m_Device->CreateFramebuffer(framebufferSpec);
    Ref<CommandList> commandList = m_Device->CreateCommandList();

    Nexus::Graphics::CubemapSpecification cubemapSpec{};
    cubemapSpec.Width = size;
    cubemapSpec.Height = size;
    cubemapSpec.MipLevels = 1;
    cubemapSpec.Format = Nexus::Graphics::PixelFormat::R32_G32_B32_A32_Float;
    Ref<Cubemap> cubemap = m_Device->CreateCubemap(cubemapSpec);

    Nexus::Graphics::PipelineDescription pipelineDescription;
    pipelineDescription.RasterizerStateDesc.TriangleCullMode = Nexus::Graphics::CullMode::Back;
    pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::CounterClockwise;
    pipelineDescription.VertexModule = m_Device->CreateShaderModuleFromSpirvFile("resources/engine/shaders/hdri.vert.glsl", Nexus::Graphics::ShaderStage::Vertex);
    pipelineDescription.FragmentModule = m_Device->CreateShaderModuleFromSpirvFile("resources/engine/shaders/hdri.frag.glsl", Nexus::Graphics::ShaderStage::Fragment);
    pipelineDescription.ResourceSetSpec.UniformBuffers = {{"Camera", 0, 0}};
    pipelineDescription.ResourceSetSpec.SampledImages = {{"equirectangularMap", 1, 0}};

    pipelineDescription.ColourFormats[0] = Nexus::Graphics::PixelFormat::R32_G32_B32_A32_Float;
    pipelineDescription.ColourTargetCount = 1;
    pipelineDescription.DepthFormat = Nexus::Graphics::PixelFormat::D32_Float_S8_UInt;

    pipelineDescription.Layouts = {Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout()};
    Ref<Pipeline> pipeline = m_Device->CreatePipeline(pipelineDescription);
    Ref<ResourceSet> resourceSet = m_Device->CreateResourceSet(pipeline);

    Nexus::Graphics::SamplerSpecification samplerSpec{};
    samplerSpec.AddressModeU = Nexus::Graphics::SamplerAddressMode::Clamp;
    samplerSpec.AddressModeV = Nexus::Graphics::SamplerAddressMode::Clamp;
    samplerSpec.AddressModeW = Nexus::Graphics::SamplerAddressMode::Clamp;
    Ref<Sampler> sampler = m_Device->CreateSampler(samplerSpec);

    Nexus::Graphics::MeshFactory factory(m_Device);
    Nexus::Ref<Nexus::Graphics::Mesh> cube = factory.CreateCube();

    VB_UNIFORM_HDRI_PROCESSOR_CAMERA cameraUniforms;

    Nexus::Graphics::BufferDescription cameraUniformBufferDesc;
    cameraUniformBufferDesc.Size = sizeof(VB_UNIFORM_HDRI_PROCESSOR_CAMERA);
    cameraUniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
    Ref<UniformBuffer> uniformBuffer = m_Device->CreateUniformBuffer(cameraUniformBufferDesc, nullptr);

    for (uint32_t i = 0; i < 6; i++)
    {
        CubemapFace face = (CubemapFace)i;
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

        float yaw = 0.0f, pitch = 0.0f;
        GetDirection(face, yaw, pitch);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::quat rotP = glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::quat rotY = glm::angleAxis(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 view = glm::mat4_cast(rotY) * glm::mat4_cast(rotP);

        float fov = 90.0f;
        float aspectRatio = 1.0f;
        float near = 0.1f;
        float far = 100.0f;
        glm::mat4 projection = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);

        cameraUniforms.View = view;
        cameraUniforms.Projection = projection;

        uniformBuffer->SetData(&cameraUniforms, sizeof(cameraUniforms), 0);
        resourceSet->WriteUniformBuffer(uniformBuffer, "Camera");
        resourceSet->WriteCombinedImageSampler(m_HdriImage, sampler, "equirectangularMap");

        commandList->Begin();
        commandList->SetPipeline(pipeline);
        commandList->SetRenderTarget(Nexus::Graphics::RenderTarget(framebuffer));

        Nexus::Graphics::Viewport vp{};
        vp.X = 0;
        vp.Y = 0;
        vp.Width = size;
        vp.Height = size;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        commandList->SetViewport(vp);

        Nexus::Graphics::Scissor scissor{};
        scissor.X = 0;
        scissor.Y = 0;
        scissor.Width = size;
        scissor.Height = size;
        commandList->SetScissor(scissor);

        commandList->SetResourceSet(resourceSet);

        commandList->SetVertexBuffer(cube->GetVertexBuffer(), 0);
        commandList->SetIndexBuffer(cube->GetIndexBuffer());
        auto indexCount = cube->GetIndexBuffer()->GetDescription().Size / sizeof(unsigned int);
        commandList->DrawIndexed(indexCount, 0, 0);

        commandList->End();
        m_Device->SubmitCommandList(commandList);

        Ref<Texture2D> colourTexture = framebuffer->GetColorTexture(0);
        std::vector<std::byte> pixels = colourTexture->GetData(0, 0, 0, size, size);

        cubemap->SetData(pixels.data(), face, 0, 0, 0, size, size);
    }

    return cubemap;
}

void HdriProcessor::GetDirection(CubemapFace face, float &yaw, float &pitch)
{
    switch (face)
    {
    case CubemapFace::PositiveX:
        pitch = 0;
        yaw = 90;
        return;
    case CubemapFace::NegativeX:
        pitch = 0;
        yaw = -90;
        return;
    case CubemapFace::PositiveY:
        pitch = 90;
        yaw = 180;
        return;
    case CubemapFace::NegativeY:
        pitch = -90;
        yaw = 180;
        return;
    case CubemapFace::PositiveZ:
        pitch = 0;
        yaw = 180;
        return;
    case CubemapFace::NegativeZ:
        pitch = 0;
        yaw = 0;
        return;
    default:
        throw std::runtime_error("Failed to find a valid face");
    }
}
} // namespace Nexus::Graphics