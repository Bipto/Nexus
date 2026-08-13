#pragma once

#include "Demo.hpp"

#include "Nexus-Core/Utils/GraphicsUtils.hpp"

namespace Demos
{
    class FramebufferDemo : public Demo
    {
      public:
        FramebufferDemo(
            const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer,
            Nexus::Graphics::CommandQueueHandle commandQueue
        )
            : Demo(name, app, imGuiRenderer, commandQueue)
        {
        }

        virtual ~FramebufferDemo()
        {
        }

        virtual void Load() override
        {
            m_CommandList = m_CommandQueue->CreateCommandList();

            Nexus::Graphics::FramebufferTextureCreateDescription framebufferDesc;
            framebufferDesc.Width = 1280;
            framebufferDesc.Height = 720;
            framebufferDesc.ColourAttachmentFormats = {Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm};
            framebufferDesc.Samples = 1;
            m_Framebuffer = Nexus::Utils::CreateFramebuffer(m_GraphicsDevice, framebufferDesc);

            Nexus::Graphics::TextureHandle texture = m_Framebuffer->GetColorTextureHandle(0);

            Nexus::Graphics::TextureViewDescription viewDesc = {};
            viewDesc.TargetTexture = texture;
            viewDesc.Format = texture->GetPixelFormat();
            viewDesc.Range = {.BaseMipLevel = 0, .LevelCount = 1, .BaseArrayLayer = 0, .LayerCount = 1};
            viewDesc.DebugName = "Framebuffer Texture View";
            m_TextureView = m_GraphicsDevice->CreateTextureView(viewDesc);

            m_TextureID = m_ImGuiRenderer->BindTexture(m_TextureView);
        }

        virtual void Render(Nexus::TimeSpan time) override
        {
            m_CommandList->Begin();
            m_CommandList->SetFramebuffer(m_Framebuffer);
            m_CommandList->ClearColourTarget(
                0, {m_RenderTargetClearColour.r, m_RenderTargetClearColour.g, m_RenderTargetClearColour.b, 1.0f}
            );
            m_CommandList->End();
            m_CommandQueue->SubmitCommandLists(&m_CommandList, 1);

            m_CommandList->Begin();

            Nexus::Graphics::SwapchainHandle swapchain = Nexus::GetApplication()->GetPrimarySwapchain();
            Nexus::Graphics::FramebufferHandle framebuffer = swapchain->GetCurrentFramebuffer();
            m_CommandList->SetFramebuffer(framebuffer);

            m_CommandList->ClearColourTarget(0, {m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, 1.0f});
            m_CommandList->End();
            m_CommandQueue->SubmitCommandLists(&m_CommandList, 1);
        }

        virtual void RenderUI() override
        {
            ImGui::ColorEdit3("Clear Color", glm::value_ptr(m_RenderTargetClearColour));
            ImGui::Image(m_TextureID, {256, 256});
        }

        virtual std::string GetInfo() const override
        {
            return "Rendering into a framebuffer and displaying the texture onto "
                   "the screen";
        }

      private:
        Nexus::Graphics::CommandListHandle m_CommandList = {};
        glm::vec3 m_ClearColour = {100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f};

        Nexus::Graphics::FramebufferHandle m_Framebuffer = {};
        ImTextureID m_TextureID = 0;
        glm::vec3 m_RenderTargetClearColour = {0.75f, 0.35f, 0.42f};

        Nexus::Graphics::TextureViewHandle m_TextureView = {};
    };
} // namespace Demos