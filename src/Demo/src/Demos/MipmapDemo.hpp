#pragma once

#include "Demo.hpp"
#include "Nexus-Core/Graphics/MipmapGenerator.hpp"

namespace Demos
{
    class MipmapDemo : public Demo
    {
      public:
        MipmapDemo(
            const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer,
            Nexus::Graphics::CommandQueueHandle commandQueue
        )
            : Demo(name, app, imGuiRenderer, commandQueue)
        {
        }

        virtual ~MipmapDemo()
        {
        }

        virtual void Load() override
        {
            m_CommandList = m_CommandQueue->CreateCommandList();

            CreatePipeline();

            Nexus::Graphics::MeshFactory factory(m_GraphicsDevice, m_CommandQueue);
            m_Mesh = factory.CreateSprite();

            auto [texture, textureView] = Nexus::Utils::CreateTexture2DWithView(
                m_CommandQueue, Nexus::FileSystem::GetFilePathAbsolute("resources/demo/textures/brick.jpg"), true
            );

            m_Texture = texture;
            m_TextureView = textureView;

            m_TextureID = m_ImGuiRenderer->BindTexture(m_TextureView);
        }

        virtual void Render(Nexus::TimeSpan time) override
        {
            auto [width, height] = m_Window->GetWindowSize();

            Nexus::Graphics::SamplerDescription samplerSpec{};
            samplerSpec.MinimumLOD = m_SelectedMip;
            samplerSpec.MaximumLOD = m_SelectedMip;
            Nexus::Graphics::SamplerHandle sampler = m_GraphicsDevice->CreateSampler(samplerSpec);

            Nexus::Graphics::CombinedImageSampler ciSampler = {};
            ciSampler.ImageTexture = m_TextureView;
            ciSampler.ImageSampler = sampler;
            m_ResourceSet->WriteCombinedImageSampler(ciSampler, "u_Texture");
            m_ResourceSet->Flush();

            m_CommandList->Begin();
            m_CommandList->SetPipeline(m_Pipeline);

            Nexus::Graphics::SwapchainHandle swapchain = Nexus::GetApplication()->GetPrimarySwapchain();
            Nexus::Graphics::FramebufferHandle framebuffer = swapchain->GetCurrentFramebuffer();
            m_CommandList->SetFramebuffer(framebuffer);

            Nexus::Graphics::Viewport vp;
            vp.X = 0;
            vp.Y = 0;
            vp.Width = width;
            vp.Height = height;
            vp.MinDepth = 0.0f;
            vp.MaxDepth = 1.0f;
            m_CommandList->SetViewport(vp);

            Nexus::Graphics::Scissor scissor;
            scissor.X = 0;
            scissor.Y = 0;
            scissor.Width = width;
            scissor.Height = height;
            m_CommandList->SetScissor(scissor);

            m_CommandList->ClearColourTarget(0, {m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, 1.0f});

            Nexus::Graphics::ResourceSetBindingDescription resourceBindingDesc = {};
            resourceBindingDesc.TargetResourceSet = m_ResourceSet;
            resourceBindingDesc.DynamicOffsets = {};
            m_CommandList->SetResourceSet(resourceBindingDesc);

            Nexus::Graphics::VertexBufferView vertexBufferView = {};
            vertexBufferView.BufferHandle = m_Mesh->GetVertexBuffer();
            vertexBufferView.Offset = 0;
            vertexBufferView.Size = m_Mesh->GetVertexBuffer()->GetSizeInBytes();
            m_CommandList->SetVertexBuffer(vertexBufferView, 0);

            Nexus::Graphics::IndexBufferView indexBufferView = {};
            indexBufferView.BufferHandle = m_Mesh->GetIndexBuffer();
            indexBufferView.Offset = 0;
            indexBufferView.Size = m_Mesh->GetIndexBuffer()->GetSizeInBytes();
            indexBufferView.BufferFormat = Nexus::Graphics::IndexFormat::UInt32;
            m_CommandList->SetIndexBuffer(indexBufferView);

            auto indexCount = m_Mesh->GetIndexBuffer()->GetCount();

            Nexus::Graphics::DrawIndexedDescription drawDesc = {};
            drawDesc.VertexStart = 0;
            drawDesc.IndexStart = 0;
            drawDesc.InstanceStart = 0;
            drawDesc.IndexCount = indexCount;
            drawDesc.InstanceCount = 1;
            m_CommandList->DrawIndexed(drawDesc);

            m_CommandList->End();

            m_CommandQueue->SubmitCommandLists(&m_CommandList, 1);
            m_GraphicsDevice->WaitForIdle();
        }

        virtual void RenderUI() override
        {
            ImGui::Image(m_TextureID, {256, 256});
            ImGui::DragInt("Mip", &m_SelectedMip, 1.0f, 0, m_Texture->GetDescription().MipLevels);
        }

        virtual std::string GetInfo() const override
        {
            return "Dynamically switching which mip level to use.";
        }

      private:
        void CreatePipeline()
        {
            Nexus::Graphics::GraphicsPipelineDescription pipelineDescription;
            pipelineDescription.RasterizerStateDesc.TriangleCullMode = Nexus::Graphics::CullMode::CullNone;
            pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::CounterClockwise;

            pipelineDescription.VertexModule = Nexus::Utils::GetOrCreateCachedShaderFromSpirvFile(
                m_GraphicsDevice, "resources/demo/shaders/texturing/texturing.vert.glsl",
                Nexus::GetApplication()->GetApplicationPath(), Nexus::Graphics::ShaderStage::Vertex
            );
            pipelineDescription.FragmentModule = Nexus::Utils::GetOrCreateCachedShaderFromSpirvFile(
                m_GraphicsDevice, "resources/demo/shaders/texturing/texturing.frag.glsl",
                Nexus::GetApplication()->GetApplicationPath(), Nexus::Graphics::ShaderStage::Fragment
            );

            pipelineDescription.ColourTargetCount = 1;
            pipelineDescription.ColourFormats[0] = Nexus::GetApplication()->GetPrimarySwapchain()->GetColourFormat();
            pipelineDescription.Samples = Nexus::GetApplication()->GetPrimarySwapchain()->GetDescription().Samples;
            pipelineDescription.Layouts = {Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout()};

            pipelineDescription.ResourceDescription.Descriptors = {Nexus::Graphics::ResourceDescriptor{
                .Name = "u_Texture",
                .Type = Nexus::Graphics::ResourceDescriptorType::CombinedImageSampler,
                .CountOrSizeInBytes = 1
            }};

            m_Pipeline = m_GraphicsDevice->CreateGraphicsPipeline(pipelineDescription);
            m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
        }

      private:
        Nexus::Graphics::CommandListHandle m_CommandList = {};
        Nexus::Graphics::PipelineHandle m_Pipeline = {};
        Nexus::Graphics::ResourceSetHandle m_ResourceSet = {};
        Nexus::Ref<Nexus::Graphics::Mesh> m_Mesh = {};
        Nexus::Graphics::TextureHandle m_Texture = {};
        Nexus::Graphics::TextureViewHandle m_TextureView = {};
        glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};

        ImTextureID m_TextureID = 0;
        int m_SelectedMip = 0;
    };
} // namespace Demos