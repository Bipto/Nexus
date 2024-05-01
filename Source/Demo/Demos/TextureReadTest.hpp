#pragma once

#include "Demo.hpp"

#include "stb_image_write.h"

namespace Demos
{
    class TextureReadTest : public Demo
    {
    public:
        TextureReadTest(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
            : Demo(name, app, imGuiRenderer)
        {
            m_CommandList = m_GraphicsDevice->CreateCommandList();

            Nexus::Graphics::FramebufferSpecification framebufferSpec = {};
            framebufferSpec.Width = 500;
            framebufferSpec.Height = 500;
            framebufferSpec.ColorAttachmentSpecification =
                {
                    {Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm}};
            framebufferSpec.Samples = Nexus::Graphics::SampleCount::SampleCount1;

            m_Framebuffer = m_GraphicsDevice->CreateFramebuffer(framebufferSpec);

            m_CommandList->Begin();
            m_CommandList->SetRenderTarget({m_Framebuffer});
            m_CommandList->ClearColorTarget(0, {1.0f, 0.0f, 0.0f, 1.0f});
            m_CommandList->End();
            m_GraphicsDevice->SubmitCommandList(m_CommandList);

            Nexus::Ref<Nexus::Graphics::Texture> colourTexture = m_Framebuffer->GetColorTexture(0);
            uint32_t textureWidth = colourTexture->GetTextureSpecification().Width;
            uint32_t textureHeight = colourTexture->GetTextureSpecification().Height;
            const std::vector<std::byte> &pixels = colourTexture->GetData(0, 0, 0, textureWidth, textureHeight);
            stbi_write_png("output.png", textureWidth, textureHeight, 4, pixels.data(), textureWidth * 4);
        }

        virtual ~TextureReadTest()
        {
        }

        virtual void Update(Nexus::Time time) override
        {
        }

        virtual void Render(Nexus::Time time) override
        {
            Nexus::Ref<Nexus::Graphics::Texture> colourTexture = m_Framebuffer->GetColorTexture(0);
            uint32_t textureWidth = colourTexture->GetTextureSpecification().Width;
            uint32_t textureHeight = colourTexture->GetTextureSpecification().Height;

            const std::vector<std::byte> &pixels = colourTexture->GetData(0, 0, 0, textureWidth, textureHeight);

            m_CommandList->Begin();
            m_CommandList->SetRenderTarget({m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()});
            m_CommandList->ClearColorTarget(0, {m_ClearColour.r,
                                                m_ClearColour.g,
                                                m_ClearColour.b,
                                                1.0f});
            m_CommandList->End();
            m_GraphicsDevice->SubmitCommandList(m_CommandList);
        }

        virtual void OnResize(Nexus::Point<uint32_t> size) override
        {
        }

        virtual void RenderUI() override
        {
        }

    private:
        Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
        Nexus::Ref<Nexus::Graphics::Framebuffer> m_Framebuffer;
        glm::vec3 m_ClearColour = {100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f};
    };
}