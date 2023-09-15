#pragma once

#include "Demo.hpp"

namespace Demos
{
    class TexturingDemo : public Demo
    {
    public:
        TexturingDemo(const std::string &name, Nexus::Application *app)
            : Demo(name, app)
        {
            Nexus::Graphics::RenderPassSpecification spec;
            m_RenderPass = m_GraphicsDevice->CreateRenderPass(spec, m_GraphicsDevice->GetSwapchain());

            m_CommandList = m_GraphicsDevice->CreateCommandList();

            m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile("Resources/Shaders/texturing.glsl",
                                                                   Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout());

            CreatePipeline();

            Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
            m_Mesh = factory.CreateSprite();

            m_Texture = m_GraphicsDevice->CreateTexture("Resources/Textures/brick.jpg");
        }

        virtual void Update(Nexus::Time time) override
        {
        }

        virtual void Render(Nexus::Time time) override
        {
            Nexus::Graphics::RenderPassBeginInfo beginInfo{};
            beginInfo.ClearColorValue = {
                m_ClearColour.r,
                m_ClearColour.g,
                m_ClearColour.b,
                1.0f};

            m_CommandList->Begin();
            m_CommandList->BeginRenderPass(m_RenderPass, beginInfo);
            m_CommandList->SetPipeline(m_Pipeline);

            m_ResourceSet->WriteTexture(m_Texture, 0);
            m_CommandList->SetResourceSet(m_ResourceSet);

            m_CommandList->SetVertexBuffer(m_Mesh.GetVertexBuffer());
            m_CommandList->SetIndexBuffer(m_Mesh.GetIndexBuffer());

            auto indexCount = m_Mesh.GetIndexBuffer()->GetDescription().Size / sizeof(unsigned int);
            m_CommandList->DrawIndexed(indexCount, 0);
            m_CommandList->EndRenderPass();
            m_CommandList->End();

            m_GraphicsDevice->SubmitCommandList(m_CommandList);
        }

        virtual void OnResize(Nexus::Point<int> size) override
        {
            CreatePipeline();
        }

        virtual void RenderUI() override
        {
        }

    private:
        void CreatePipeline()
        {
            Nexus::Graphics::PipelineDescription pipelineDescription;
            pipelineDescription.RasterizerStateDescription.CullMode = Nexus::Graphics::CullMode::Back;
            pipelineDescription.RasterizerStateDescription.FrontFace = Nexus::Graphics::FrontFace::CounterClockwise;
            pipelineDescription.Shader = m_Shader;

            pipelineDescription.Viewport = {
                0, 0, m_Window->GetWindowSize().X, m_Window->GetWindowSize().Y};

            Nexus::Graphics::TextureResourceBinding textureBinding;
            textureBinding.Slot = 0;
            textureBinding.Name = "texSampler";
            pipelineDescription.ResourceSetSpecification.TextureBindings = {textureBinding};

            m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDescription);
            m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
        }

    private:
        Nexus::Ref<Nexus::Graphics::RenderPass> m_RenderPass;
        Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
        Nexus::Ref<Nexus::Graphics::Shader> m_Shader;
        Nexus::Ref<Nexus::Graphics::Pipeline> m_Pipeline;
        Nexus::Ref<Nexus::Graphics::ResourceSet> m_ResourceSet;
        Nexus::Graphics::Mesh m_Mesh;
        Nexus::Ref<Nexus::Graphics::Texture> m_Texture;
        glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};
    };
}