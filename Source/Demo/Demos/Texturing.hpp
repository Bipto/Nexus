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

            m_CommandList = m_GraphicsDevice->CreateCommandList();
            m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile(Nexus::FileSystem::GetFilePathAbsolute("resources/shaders/texturing.glsl"),
                                                                   Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout());

            CreatePipeline();

            Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
            m_Mesh = factory.CreateSprite();

            m_Texture = m_GraphicsDevice->CreateTexture(Nexus::FileSystem::GetFilePathAbsolute("resources/textures/brick.jpg"));
        }

        virtual ~TexturingDemo()
        {
            delete m_CommandList;
            delete m_Shader;
            delete m_Pipeline;
            delete m_ResourceSet;
            delete m_Mesh;
            delete m_Texture;
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

            m_ResourceSet->WriteTexture(m_Texture, 0);
            m_CommandList->SetResourceSet(m_ResourceSet);

            m_CommandList->SetVertexBuffer(m_Mesh->GetVertexBuffer());
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
        }

    private:
        void CreatePipeline()
        {
            Nexus::Graphics::PipelineDescription pipelineDescription;
            pipelineDescription.RasterizerStateDescription.CullMode = Nexus::Graphics::CullMode::None;
            pipelineDescription.RasterizerStateDescription.FrontFace = Nexus::Graphics::FrontFace::CounterClockwise;
            pipelineDescription.Shader = m_Shader;

            Nexus::Graphics::TextureResourceBinding textureBinding;
            textureBinding.Slot = 0;
            textureBinding.Name = "texSampler";
            pipelineDescription.ResourceSetSpecification.TextureBindings = {textureBinding};

            pipelineDescription.Target = {m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()};

            m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDescription);
            m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
        }

    private:
        Nexus::Graphics::CommandList *m_CommandList;
        Nexus::Graphics::Shader *m_Shader;
        Nexus::Graphics::Pipeline *m_Pipeline;
        Nexus::Graphics::ResourceSet *m_ResourceSet;
        Nexus::Graphics::Mesh *m_Mesh;
        Nexus::Graphics::Texture *m_Texture;
        glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};
    };
}