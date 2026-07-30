#include "Nexus-Core/Graphics/HdriProcessor.hpp"

#include "Nexus-Core/Graphics/MeshFactory.hpp"
#include "Nexus-Core/Runtime.hpp"
#include "Nexus-Core/Utils/GraphicsUtils.hpp"
#include "RHI/Framebuffer.hpp"
#include "RHI/GraphicsDevice.hpp"
#include "RHI/Texture.hpp"

#include "Nexus-Core/Utils/Utils.hpp"

#include "stb_image.h"
#include "stb_image_write.h"

const std::string HdriVertexShaderSource = "#version 450 core\n"

                                           "layout(location = 0) in vec3 a_Position;\n"

                                           "layout(location = 0) out vec3 o_LocalPos;\n"

                                           "layout(binding = 0, set = 0) uniform Camera\n"
                                           "{\n"
                                           "    mat4 u_View;\n"
                                           "    mat4 u_Projection;\n"
                                           "};\n"

                                           "void main()\n"
                                           "{\n"
                                           "    o_LocalPos = a_Position;\n"
                                           "    gl_Position = u_Projection * u_View * vec4(a_Position, 1.0);\n"
                                           "}";

const std::string HdriFragmentShaderSource = "#version 450 core\n"

                                             "layout(location = 0) in vec3 a_LocalPos;\n"

                                             "layout(location = 0) out vec4 o_Colour;\n"

                                             "layout(binding = 0, set = 1) uniform sampler2D u_EquirectangularMap;\n"

                                             "const vec2 invAtan = vec2(0.1591, 0.3183);\n"
                                             "vec2 SampleSphericalMap(vec3 v)\n"
                                             "{\n"
                                             "    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));\n"
                                             "    uv *= invAtan;\n"
                                             "    uv += 0.5;\n"
                                             "    return uv;\n"
                                             "}\n"

                                             "void main()\n"
                                             "{\n"
                                             "    vec2 uv = SampleSphericalMap(normalize(a_LocalPos));\n"
                                             "    vec3 colour = texture(u_EquirectangularMap, uv).rgb;\n"
                                             "    o_Colour = vec4(colour, 1.0);\n"
                                             "}";

namespace Nexus::Graphics
{
    struct alignas(16) VB_UNIFORM_HDRI_PROCESSOR_CAMERA
    {
        glm::mat4 View = {};
        glm::mat4 Projection = {};
    };

    HdriProcessor::HdriProcessor(const std::string &filepath, IGraphicsDevice *device, CommandQueueHandle commandQueue)
        : m_Device(device), m_CommandQueue(commandQueue)
    {
        stbi_set_flip_vertically_on_load(true);
        int channels = 0;
        int requestedChannels = 4;
        float *data = stbi_loadf(filepath.c_str(), &m_Width, &m_Height, &channels, requestedChannels);

        std::vector<unsigned char> pixels;
        size_t bufferSize = m_Width * m_Height * requestedChannels * sizeof(float);
        pixels.resize(bufferSize);
        memcpy(pixels.data(), data, pixels.size());
        stbi_image_free(data);

        if (m_Device->GetGraphicsAPI().API == GraphicsAPI::OpenGL)
        {
            Utils::FlipPixelsHorizontally(
                pixels.data(), m_Width, m_Height, Graphics::PixelFormat::R32_G32_B32_A32_Float
            );
        }

        Graphics::TextureDescription textureDesc = {};
        textureDesc.Width = m_Width;
        textureDesc.Height = m_Height;
        textureDesc.DepthOrArrayLayers = 1;
        textureDesc.MipLevels = 1;
        textureDesc.Usage = Nexus::Graphics::TextureUsage_Sampled;
        textureDesc.Type = Graphics::TextureType::Texture2D;
        textureDesc.Format = Graphics::PixelFormat::R32_G32_B32_A32_Float;
        textureDesc.DebugName = "HDRI";
        m_HdriImage = m_Device->CreateTexture(textureDesc);
        Utils::WriteToTexture(m_CommandQueue, m_HdriImage, 0, 0, 0, 0, m_Width, m_Height, pixels.data(), pixels.size());

        Graphics::TextureViewDescription cubemapViewDesc = {};
        cubemapViewDesc.TargetTexture = m_HdriImage;
        cubemapViewDesc.Format = m_HdriImage->GetPixelFormat();
        cubemapViewDesc.Range = {
            .BaseMipLevel = 0,
            .LevelCount = m_HdriImage->GetMipLevels(),
            .BaseArrayLayer = 0,
            .LayerCount = m_HdriImage->GetDepthOrArrayLayers()
        };
        cubemapViewDesc.DebugName = "HDRI View";
        m_HdriView = m_Device->CreateTextureView(cubemapViewDesc);
    }

    TextureHandle HdriProcessor::Generate(uint32_t size)
    {
        Nexus::Graphics::FramebufferTextureCreateDescription framebufferSpec = {};
        framebufferSpec.Width = size;
        framebufferSpec.Height = size;
        framebufferSpec.Samples = 1;
        framebufferSpec.ColourAttachmentFormats = {PixelFormat::R32_G32_B32_A32_Float};
        framebufferSpec.DepthAttachmentFormat = PixelFormat::D24_UNorm_S8_UInt;

        FramebufferHandle framebuffer = Utils::CreateFramebuffer(m_Device, framebufferSpec);
        CommandListHandle commandList = m_CommandQueue->CreateCommandList();

        Graphics::TextureDescription cubemapSpec = {};
        cubemapSpec.Type = Graphics::TextureType::TextureCube;
        cubemapSpec.Usage = Graphics::TextureUsage_Sampled;
        cubemapSpec.Format = Graphics::PixelFormat::R32_G32_B32_A32_Float;
        cubemapSpec.Width = size;
        cubemapSpec.Height = size;
        cubemapSpec.MipLevels = 1;
        cubemapSpec.DepthOrArrayLayers = 6;
        cubemapSpec.DebugName = "Cubemap";
        TextureHandle cubemap = m_Device->CreateTexture(cubemapSpec);

        Nexus::Graphics::GraphicsPipelineDescription pipelineDescription;
        pipelineDescription.RasterizerStateDesc.TriangleCullMode = Nexus::Graphics::CullMode::Back;
        pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::CounterClockwise;

        pipelineDescription.VertexModule = Nexus::Utils::CreateShaderModuleFromSpirvSource(
            m_Device, HdriVertexShaderSource, "hdri.vert.glsl", Nexus::GetApplication()->GetApplicationPath(),
            Nexus::Graphics::ShaderStage::Vertex
        );

        pipelineDescription.FragmentModule = Nexus::Utils::CreateShaderModuleFromSpirvSource(
            m_Device, HdriFragmentShaderSource, "hdri.frag.glsl", Nexus::GetApplication()->GetApplicationPath(),
            Nexus::Graphics::ShaderStage::Fragment
        );

        pipelineDescription.ColourFormats[0] = framebufferSpec.ColourAttachmentFormats[0];
        pipelineDescription.ColourTargetCount = 1;
        pipelineDescription.DepthFormat = framebufferSpec.DepthAttachmentFormat.value();

        pipelineDescription.Layouts = {Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout()};

        pipelineDescription.ResourceDescription.Descriptors = {
            Nexus::Graphics::ResourceDescriptor{
                .Name = "u_EquirectangularMap",
                .Type = Nexus::Graphics::ResourceDescriptorType::CombinedImageSampler,
                .CountOrSizeInBytes = 1
            },
            Nexus::Graphics::ResourceDescriptor{
                .Name = "Camera",
                .Type = Nexus::Graphics::ResourceDescriptorType::UniformBuffer,
                .CountOrSizeInBytes = 1
            }
        };

        PipelineHandle pipeline = m_Device->CreateGraphicsPipeline(pipelineDescription);
        ResourceSetHandle resourceSet = m_Device->CreateResourceSet(pipeline);

        Nexus::Graphics::SamplerDescription samplerSpec{};
        samplerSpec.AddressModeU = Nexus::Graphics::SamplerAddressMode::Clamp;
        samplerSpec.AddressModeV = Nexus::Graphics::SamplerAddressMode::Clamp;
        samplerSpec.AddressModeW = Nexus::Graphics::SamplerAddressMode::Clamp;
        SamplerHandle sampler = m_Device->CreateSampler(samplerSpec);

        Nexus::Graphics::MeshFactory factory(m_Device, m_CommandQueue);
        Nexus::Ref<Nexus::Graphics::Mesh> cube = factory.CreateCube();

        VB_UNIFORM_HDRI_PROCESSOR_CAMERA cameraUniforms;

        Nexus::Graphics::DeviceBufferDescription cameraUniformBufferDesc = {};
        cameraUniformBufferDesc.Access = BufferMemoryAccess::Upload;
        cameraUniformBufferDesc.Usage = Nexus::Graphics::BufferUsage_Uniform;
        cameraUniformBufferDesc.StrideInBytes = sizeof(VB_UNIFORM_HDRI_PROCESSOR_CAMERA);
        cameraUniformBufferDesc.SizeInBytes = sizeof(VB_UNIFORM_HDRI_PROCESSOR_CAMERA);
        Graphics::DeviceBufferHandle uniformBuffer = m_Device->CreateDeviceBuffer(cameraUniformBufferDesc);

        for (uint32_t i = 0; i < 6; i++)
        {
            glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

            float yaw = 0.0f, pitch = 0.0f;
            GetDirection(i, yaw, pitch, m_Device->IsUVOriginTopLeft());

            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

            glm::quat rotP = glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
            glm::quat rotY = glm::angleAxis(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 view = glm::mat4_cast(rotY) * glm::mat4_cast(rotP);

            if (!m_Device->IsUVOriginTopLeft())
            {
                view *= glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            }

            float fov = 90.0f;
            float aspectRatio = 1.0f;
            float camNear = 0.1f;
            float camFar = 100.0f;
            glm::mat4 projection = glm::perspective(glm::radians(fov), aspectRatio, camNear, camFar);

            cameraUniforms.View = view;
            cameraUniforms.Projection = projection;

            uniformBuffer->SetData(&cameraUniforms, 0, sizeof(cameraUniforms));

            UniformBufferView uniformBufferView = {};
            uniformBufferView.BufferHandle = uniformBuffer;
            uniformBufferView.Offset = 0;
            uniformBufferView.Size = uniformBuffer->GetDescription().SizeInBytes;
            resourceSet->WriteUniformBuffer(uniformBufferView, "Camera");

            CombinedImageSampler ciSampler = {};
            ciSampler.ImageTexture = m_HdriView;
            ciSampler.ImageSampler = sampler;
            resourceSet->WriteCombinedImageSampler(ciSampler, "u_EquirectangularMap");

            resourceSet->Flush();

            commandList->Begin();
            commandList->SetPipeline(pipeline);
            commandList->SetFramebuffer(framebuffer);

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

            Nexus::Graphics::ResourceSetBindingDescription resourceBindingDesc = {};
            resourceBindingDesc.TargetResourceSet = resourceSet;
            resourceBindingDesc.DynamicOffsets = {};
            commandList->SetResourceSet(resourceBindingDesc);

            Graphics::VertexBufferView vertexBufferView = {};
            vertexBufferView.BufferHandle = cube->GetVertexBuffer();
            vertexBufferView.Offset = 0;
            vertexBufferView.Size = cube->GetVertexBuffer()->GetSizeInBytes();
            commandList->SetVertexBuffer(vertexBufferView, 0);

            Graphics::IndexBufferView indexBufferView = {};
            indexBufferView.BufferHandle = cube->GetIndexBuffer();
            indexBufferView.Offset = 0;
            indexBufferView.Size = cube->GetIndexBuffer()->GetSizeInBytes();
            indexBufferView.BufferFormat = Nexus::Graphics::IndexFormat::UInt32;
            commandList->SetIndexBuffer(indexBufferView);

            auto indexCount = cube->GetIndexBuffer()->GetCount();

            Nexus::Graphics::DrawIndexedDescription drawDesc = {};
            drawDesc.VertexStart = 0;
            drawDesc.IndexStart = 0;
            drawDesc.InstanceStart = 0;
            drawDesc.IndexCount = indexCount;
            drawDesc.InstanceCount = 1;
            commandList->DrawIndexed(drawDesc);

            commandList->End();

            m_CommandQueue->SubmitCommandLists(&commandList, 1);
            m_Device->WaitForIdle();

            TextureHandle colourTexture = framebuffer->GetColorTextureHandle(0);
            std::vector<char> pixels = Utils::ReadFromTexture(m_CommandQueue, colourTexture, 0, 0, 0, 0, size, size);

            Utils::WriteToTexture(m_CommandQueue, cubemap, 0, 0, 0, i, size, size, pixels.data(), pixels.size());
        }

        return cubemap;
    }

    TextureViewHandle HdriProcessor::GenerateView(uint32_t size)
    {
        TextureHandle cubemap = Generate(size);

        Graphics::TextureViewDescription cubemapViewDesc = {
            .TargetTexture = cubemap,
            .Format = cubemap->GetPixelFormat(),
            .Range =
                {.BaseMipLevel = 0,
                 .LevelCount = cubemap->GetMipLevels(),
                 .BaseArrayLayer = 0,
                 .LayerCount = cubemap->GetDepthOrArrayLayers()},
            .DebugName = "Cubemap View"
        };

        return m_Device->CreateTextureView(cubemapViewDesc);
    }

    TextureHandle HdriProcessor::GetLoadedTexture() const
    {
        return m_HdriImage;
    }

    void HdriProcessor::GetDirection(uint32_t face, float &yaw, float &pitch, bool yUp)
    {
        switch (face)
        {
        // positive x
        case 0:
            pitch = 0;
            yaw = 90;
            return;
        // negative x
        case 1:
            pitch = 0;
            yaw = -90;
            return;
        // positive y
        case 2:
            if (yUp)
            {
                pitch = -90;
                yaw = 180;
            }
            else
            {
                pitch = 90;
                yaw = 180;
            }

            return;
        // negative y
        case 3:
            if (yUp)
            {
                pitch = 90;
                yaw = 180;
            }
            else
            {
                pitch = -90;
                yaw = 180;
            }

            return;
        // positive z
        case 4:
            pitch = 0;
            yaw = 180;
            return;
        // negative z
        case 5:
            pitch = 0;
            yaw = 0;
            return;
        default:
            throw std::runtime_error("Failed to find a valid face");
        }
    }
} // namespace Nexus::Graphics