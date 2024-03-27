#pragma once

#include "Demo.hpp"

namespace Demos
{
    class TexturingDemo : public Demo
    {
    public:
        TexturingDemo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
            : Demo(name, app, imGuiRenderer)
        {

            m_CommandList = m_GraphicsDevice->CreateCommandList();

            CreatePipeline();

            Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
            m_Mesh = factory.CreateSprite();

            m_Texture = m_GraphicsDevice->CreateTexture(Nexus::FileSystem::GetFilePathAbsolute("resources/textures/brick.jpg"));

            Nexus::Graphics::SamplerSpecification samplerSpec{};
            samplerSpec.MaximumAnisotropy = 8;
            m_Sampler = m_GraphicsDevice->CreateSampler(samplerSpec);

            m_TextureID = m_ImGuiRenderer->BindTexture(m_Texture);
        }

        virtual ~TexturingDemo()
        {
        }

        virtual void Update(Nexus::Time time) override
        {
        }

        virtual void Render(Nexus::Time time) override
        {
            m_CommandList->Begin();
            m_CommandList->SetPipeline(m_Pipeline);

            Nexus::Graphics::Viewport vp;
            vp.X = 0;
            vp.Y = 0;
            vp.Width = m_GraphicsDevice->GetPrimaryWindow()->GetWindowSize().X;
            vp.Height = m_GraphicsDevice->GetPrimaryWindow()->GetWindowSize().Y;
            vp.MinDepth = 0.0f;
            vp.MaxDepth = 1.0f;
            m_CommandList->SetViewport(vp);

            Nexus::Graphics::Scissor scissor;
            scissor.X = 0;
            scissor.Y = 0;
            scissor.Width = m_GraphicsDevice->GetPrimaryWindow()->GetWindowSize().X;
            scissor.Height = m_GraphicsDevice->GetPrimaryWindow()->GetWindowSize().Y;
            m_CommandList->SetScissor(scissor);

            m_CommandList->ClearColorTarget(0,
                                            {m_ClearColour.r,
                                             m_ClearColour.g,
                                             m_ClearColour.b,
                                             1.0f});

            m_ResourceSet->WriteCombinedImageSampler(m_Texture, m_Sampler, "texSampler");
            m_ResourceSet->Flush();
            m_CommandList->SetResourceSet(m_ResourceSet);

            m_CommandList->SetVertexBuffer(m_Mesh->GetVertexBuffer(), 0);
            m_CommandList->SetIndexBuffer(m_Mesh->GetIndexBuffer());

            auto indexCount = m_Mesh->GetIndexBuffer()->GetDescription().Size / sizeof(unsigned int);
            m_CommandList->DrawIndexed(indexCount, 0, 0);
            m_CommandList->End();

            m_GraphicsDevice->SubmitCommandList(m_CommandList);
        }

        virtual void OnResize(Nexus::Point<uint32_t> size) override
        {
        }

        virtual void RenderUI() override
        {
            ImGui::Image(m_TextureID, {256, 256});
        }

    private:
        void CreatePipeline()
        {
            Nexus::Graphics::PipelineDescription pipelineDescription;
            pipelineDescription.RasterizerStateDescription.CullMode = Nexus::Graphics::CullMode::None;
            pipelineDescription.RasterizerStateDescription.FrontFace = Nexus::Graphics::FrontFace::CounterClockwise;

            pipelineDescription.VertexModule = m_GraphicsDevice->CreateShaderModuleFromSpirvFile("resources/shaders/texturing.vert.glsl", Nexus::Graphics::ShaderStage::Vertex);
            pipelineDescription.FragmentModule = m_GraphicsDevice->CreateShaderModuleFromSpirvFile("resources/shaders/texturing.frag.glsl", Nexus::Graphics::ShaderStage::Fragment);

            pipelineDescription.ResourceSetSpecification.SampledImages =
                {
                    {"texSampler", 0, 0}};

            pipelineDescription.Target = {m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()};
            pipelineDescription.Layouts = {Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout()};

            m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDescription);
            m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
        }

    private:
        Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList = nullptr;
        Nexus::Ref<Nexus::Graphics::Pipeline> m_Pipeline = nullptr;
        Nexus::Ref<Nexus::Graphics::ResourceSet> m_ResourceSet = nullptr;
        Nexus::Ref<Nexus::Graphics::Mesh> m_Mesh = nullptr;
        Nexus::Ref<Nexus::Graphics::Texture> m_Texture = nullptr;
        Nexus::Ref<Nexus::Graphics::Sampler> m_Sampler = nullptr;
        glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};

        ImTextureID m_TextureID = 0;
    };
}