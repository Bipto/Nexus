#include "MipmapGenerator.hpp"

#include <algorithm>

#include "stb_image_write.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

namespace Nexus::Graphics
{
    MipmapGenerator::MipmapGenerator(GraphicsDevice *device)
        : m_Device(device), m_Quad(device)
    {
        m_CommandList = m_Device->CreateCommandList();
    }

    void MipmapGenerator::GenerateMips(Ref<Texture> texture, uint32_t mipCount)
    {
        const uint32_t textureWidth = texture->GetTextureSpecification().Width;
        const uint32_t textureHeight = texture->GetTextureSpecification().Height;

        uint32_t mipWidth = textureWidth;
        uint32_t mipHeight = textureHeight;

        Ref<ShaderModule> vertexModule = m_Device->CreateShaderModuleFromSpirvSource(c_MipmapVertexSource, "Mipmap-Gen.vert", Nexus::Graphics::ShaderStage::Vertex);
        Ref<ShaderModule> fragmentModule = m_Device->CreateShaderModuleFromSpirvSource(c_MipmapFragmentSource, "Mipmap-Gen.frag", Nexus::Graphics::ShaderStage::Fragment);

        Ref<Texture> mipTexture = texture;

        // iterate through each mip to generate
        for (uint32_t i = 1; i <= mipCount; i++)
        {
            // set up pipeline for rendering
            Nexus::Graphics::PipelineDescription pipelineDescription;
            pipelineDescription.RasterizerStateDesc.CullMode = Nexus::Graphics::CullMode::None;
            pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::CounterClockwise;

            pipelineDescription.VertexModule = vertexModule;
            pipelineDescription.FragmentModule = fragmentModule;

            pipelineDescription.ResourceSetSpec.SampledImages =
                {
                    {"texSampler", 0, 0}};

            pipelineDescription.Layouts = {m_Quad.GetVertexBufferLayout()};

            // create mip
            {
                mipWidth /= 2;
                mipHeight /= 2;

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

                Nexus::Graphics::Scissor scissor;
                scissor.X = 0;
                scissor.Y = 0;
                scissor.Width = mipWidth;
                scissor.Height = mipHeight;

                Nexus::Graphics::Viewport viewport;
                viewport.X = 0;
                viewport.Y = 0;
                viewport.Width = mipWidth;
                viewport.Height = mipHeight;
                viewport.MinDepth = 0;
                viewport.MaxDepth = 1;

                m_CommandList->Begin();
                m_CommandList->SetPipeline(pipeline);
                m_CommandList->SetViewport(viewport);
                m_CommandList->SetScissor(scissor);
                m_CommandList->SetVertexBuffer(m_Quad.GetVertexBuffer(), 0);
                m_CommandList->SetIndexBuffer(m_Quad.GetIndexBuffer());
                m_CommandList->SetResourceSet(resourceSet);
                m_CommandList->DrawIndexed(6, 0, 0);
                m_CommandList->End();
                m_Device->SubmitCommandList(m_CommandList);

                Ref<Texture> framebufferTexture = framebuffer->GetColorTexture(0);
                uint32_t framebufferWidth = framebufferTexture->GetTextureSpecification().Width;
                uint32_t framebufferHeight = framebufferTexture->GetTextureSpecification().Height;

                std::vector<std::byte> pixels = framebufferTexture->GetData(0, 0, 0, framebufferWidth, framebufferHeight);
                MipData mipData(pixels, framebufferWidth, framebufferHeight);
                texture->SetData(mipData.GetData(), i, 0, 0, framebufferWidth, framebufferHeight);

                mipTexture = framebufferTexture;
            }
        }
    }

    uint32_t MipmapGenerator::GetMaximumNumberOfMips(uint32_t width, uint32_t height)
    {
        return std::floor(std::log2(std::max(width, height)));
    }

    MipData::MipData(const std::vector<std::byte> &pixels, uint32_t width, uint32_t height)
        : m_Pixels(pixels), m_Width(width), m_Height(height)
    {
    }

    uint32_t MipData::GetWidth() const
    {
        return m_Width;
    }

    uint32_t MipData::GetHeight() const
    {
        return m_Height;
    }

    const void *MipData::GetData() const
    {
        return m_Pixels.data();
    }
}