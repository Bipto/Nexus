#pragma once

#include "Demo.hpp"

namespace Demos
{
    class HelloTriangleDemo : public Demo
    {
    public:
        HelloTriangleDemo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
            : Demo(name, app, imGuiRenderer)
        {
            m_CommandList = m_GraphicsDevice->CreateCommandList();

            m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile(Nexus::FileSystem::GetFilePathAbsolute("resources/shaders/hello_triangle.glsl"),
                                                                   Nexus::Graphics::VertexPosition::GetLayout());

            std::vector<Nexus::Graphics::VertexPosition> vertices =
                {
                    {{-0.5f, -0.5f, 0.0f}}, // bottom left
                    {{0.0f, 0.5f, 0.0f}},   // top left
                    {{0.5f, -0.5f, 0.0f}},  // bottom right
                };

            CreatePipeline();

            Nexus::Graphics::BufferDescription vertexBufferDesc;
            vertexBufferDesc.Size = vertices.size() * sizeof(Nexus::Graphics::VertexPosition);
            vertexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
            m_VertexBuffer = m_GraphicsDevice->CreateVertexBuffer(vertexBufferDesc, vertices.data(), Nexus::Graphics::VertexPosition::GetLayout());
        }

        virtual ~HelloTriangleDemo()
        {
            delete m_CommandList;
            delete m_Shader;
            delete m_Pipeline;
            delete m_VertexBuffer;
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

            m_CommandList->SetVertexBuffer(m_VertexBuffer);
            auto vertexCount = m_VertexBuffer->GetDescription().Size / sizeof(Nexus::Graphics::VertexPosition);
            m_CommandList->DrawElements(0, vertexCount);
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
            pipelineDescription.Target = {m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()};

            m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDescription);
        }

    private:
        Nexus::Graphics::CommandList *m_CommandList;
        Nexus::Graphics::Shader *m_Shader;
        Nexus::Graphics::Pipeline *m_Pipeline;
        Nexus::Graphics::VertexBuffer *m_VertexBuffer;
        glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};
    };
}