#include "MipmapGenerator.hpp"

#include <algorithm>

#include "stb_image_write.h"

const std::string c_MipmapVertexSource =
    "#version 450 core\n"
    "layout (location = 0) in vec3 Position;\n"
    "layout (location = 1) in vec2 TexCoord;\n"
    "layout (location = 0) out vec2 OutTexCoord;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(Position, 1.0);\n"
    "    OutTexCoord = TexCoord;\n"
    "}";

const std::string c_MipmapFragmentSource =
    "#version 450 core\n"
    "layout(location = 0) in vec2 OutTexCoord;\n"
    "layout(location = 0) out vec4 FragColor;\n"
    "layout(binding = 0, set = 0) uniform sampler2D texSampler;\n"
    "void main()\n"
    "{\n"
    "    FragColor = texture(texSampler, OutTexCoord);\n"
    "}";

/* const std::string c_MipmapFragmentSource =
    "#version 450 core\n"
    "layout(location = 0) in vec2 OutTexCoord;\n"
    "layout(location = 0) out vec4 FragColor;\n"
    "layout(binding = 0, set = 0) uniform sampler2D texSampler;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(OutTexCoord.x, OutTexCoord.y, 0.0, 1.0);\n"
    "}"; */

namespace Nexus::Graphics
{
    MipmapGenerator::MipmapGenerator(GraphicsDevice *device)
        : m_Device(device), m_Quad(device)
    {
        m_CommandList = m_Device->CreateCommandList();
    }

    void MipmapGenerator::GenerateMip(Ref<Texture> texture)
    {
        Nexus::Graphics::PipelineDescription pipelineDescription;
        pipelineDescription.RasterizerStateDescription.CullMode = Nexus::Graphics::CullMode::None;
        pipelineDescription.RasterizerStateDescription.FrontFace = Nexus::Graphics::FrontFace::CounterClockwise;

        pipelineDescription.VertexModule = m_Device->CreateShaderModuleFromSpirvSource(c_MipmapVertexSource, "Mipmap-Gen.vert", Nexus::Graphics::ShaderStage::Vertex);
        pipelineDescription.FragmentModule = m_Device->CreateShaderModuleFromSpirvSource(c_MipmapFragmentSource, "Mipmap-Gen.frag", Nexus::Graphics::ShaderStage::Fragment);

        pipelineDescription.ResourceSetSpecification.SampledImages =
            {
                {"texSampler", 0, 0}};

        pipelineDescription.Layouts = {m_Quad.GetVertexBufferLayout()};

        const uint32_t textureWidth = texture->GetTextureSpecification().Width;
        const uint32_t textureHeight = texture->GetTextureSpecification().Height;

        std::vector<std::byte> texData = texture->GetData(0, 0, 0, textureWidth, textureHeight);
        std::string name = "mip" + std::to_string(0) + ".png";
        stbi_write_png(name.c_str(), textureWidth, textureHeight, 4, texData.data(), textureWidth * 4);

        {
            const uint32_t mipWidth = textureWidth;
            const uint32_t mipHeight = textureHeight;

            Nexus::Graphics::FramebufferSpecification framebufferSpec;
            framebufferSpec.ColorAttachmentSpecification = {texture->GetTextureSpecification().Format};
            framebufferSpec.Width = mipWidth;
            framebufferSpec.Height = mipHeight;
            framebufferSpec.Samples = texture->GetTextureSpecification().Samples;

            Ref<Framebuffer> framebuffer = m_Device->CreateFramebuffer(framebufferSpec);

            pipelineDescription.Target = {framebuffer};
            Ref<Pipeline> pipeline = m_Device->CreatePipeline(pipelineDescription);
            Ref<ResourceSet> resourceSet = m_Device->CreateResourceSet(pipeline);

            Nexus::Graphics::SamplerSpecification samplerSpec;
            samplerSpec.MinimumLOD = 0;
            samplerSpec.MaximumLOD = 0;
            Ref<Sampler> sampler = m_Device->CreateSampler(samplerSpec);
            resourceSet->WriteCombinedImageSampler(texture, sampler, "texSampler");

            m_CommandList->Begin();

            m_CommandList->SetPipeline(pipeline);
            m_CommandList->SetVertexBuffer(m_Quad.GetVertexBuffer(), 0);
            m_CommandList->SetIndexBuffer(m_Quad.GetIndexBuffer());
            m_CommandList->SetResourceSet(resourceSet);
            m_CommandList->DrawIndexed(6, 0, 0);

            m_CommandList->End();
            m_Device->SubmitCommandList(m_CommandList);

            Ref<Texture> framebufferTexture = framebuffer->GetColorTexture(0);
            std::vector<std::byte> pixels = framebufferTexture->GetData(0, 0, 0, mipWidth, mipHeight);

            std::string name = "mip" + std::to_string(1) + ".png";
            stbi_write_png(name.c_str(), mipWidth, mipHeight, 4, pixels.data(), mipWidth * 4);
        }
    }

    std::vector<std::vector<std::byte>> MipmapGenerator::GenerateMips(Ref<Texture> texture, uint32_t mipCount)
    {
        const uint32_t textureWidth = texture->GetTextureSpecification().Width;
        const uint32_t textureHeight = texture->GetTextureSpecification().Height;

        uint32_t mipWidth = textureWidth;
        uint32_t mipHeight = textureHeight;

        std::vector<std::vector<std::byte>> mips;

        Ref<ShaderModule> vertexModule = m_Device->CreateShaderModuleFromSpirvSource(c_MipmapVertexSource, "Mipmap-Gen.vert", Nexus::Graphics::ShaderStage::Vertex);
        Ref<ShaderModule> fragmentModule = m_Device->CreateShaderModuleFromSpirvSource(c_MipmapFragmentSource, "Mipmap-Gen.frag", Nexus::Graphics::ShaderStage::Fragment);

        Ref<Texture> mipTexture = texture;

        // iterate through each mip to generate
        for (uint32_t i = 0; i < mipCount; i++)
        {
            mipWidth /= 2;
            mipHeight /= 2;

            // set up pipeline for rendering
            Nexus::Graphics::PipelineDescription pipelineDescription;
            pipelineDescription.RasterizerStateDescription.CullMode = Nexus::Graphics::CullMode::None;
            pipelineDescription.RasterizerStateDescription.FrontFace = Nexus::Graphics::FrontFace::CounterClockwise;

            pipelineDescription.VertexModule = vertexModule;
            pipelineDescription.FragmentModule = fragmentModule;

            pipelineDescription.ResourceSetSpecification.SampledImages =
                {
                    {"texSampler", 0, 0}};

            pipelineDescription.Layouts = {m_Quad.GetVertexBufferLayout()};

            // create mip
            {
                Nexus::Graphics::FramebufferSpecification framebufferSpec;
                framebufferSpec.ColorAttachmentSpecification = {texture->GetTextureSpecification().Format};
                framebufferSpec.Width = mipWidth;
                framebufferSpec.Height = mipHeight;
                framebufferSpec.Samples = texture->GetTextureSpecification().Samples;

                Ref<Framebuffer> framebuffer = m_Device->CreateFramebuffer(framebufferSpec);

                pipelineDescription.Target = {framebuffer};
                Ref<Pipeline> pipeline = m_Device->CreatePipeline(pipelineDescription);
                Ref<ResourceSet> resourceSet = m_Device->CreateResourceSet(pipeline);

                Nexus::Graphics::SamplerSpecification samplerSpec;
                samplerSpec.MinimumLOD = 0;
                samplerSpec.MaximumLOD = 0;
                Ref<Sampler> sampler = m_Device->CreateSampler(samplerSpec);
                resourceSet->WriteCombinedImageSampler(mipTexture, sampler, "texSampler");

                m_CommandList->Begin();

                m_CommandList->SetPipeline(pipeline);
                m_CommandList->SetVertexBuffer(m_Quad.GetVertexBuffer(), 0);
                m_CommandList->SetIndexBuffer(m_Quad.GetIndexBuffer());
                m_CommandList->SetResourceSet(resourceSet);
                m_CommandList->DrawIndexed(6, 0, 0);

                m_CommandList->End();
                m_Device->SubmitCommandList(m_CommandList);

                Ref<Texture> framebufferTexture = framebuffer->GetColorTexture(0);
                std::vector<std::byte> pixels = framebufferTexture->GetData(0, 0, 0, mipWidth, mipHeight);

                std::string name = "mip" + std::to_string(i) + ".png";
                stbi_write_png(name.c_str(), mipWidth, mipHeight, 4, pixels.data(), mipWidth * 4);

                mipTexture = framebufferTexture;
                mips.push_back(pixels);
            }
        }

        return mips;
    }

    uint32_t MipmapGenerator::GetMaximumNumberOfMips(uint32_t width, uint32_t height)
    {
        return 1 + std::floor(std::log2(std::max(width, height)));
    }
}