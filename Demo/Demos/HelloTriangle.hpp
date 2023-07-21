#pragma once

#include "Demo.hpp"

namespace Demos
{
    class HelloTriangleDemo : public Demo
    {
    public:
        HelloTriangleDemo(const std::string &name, Nexus::Application *app)
            : Demo(name, app)
        {
            m_CommandList = m_GraphicsDevice->CreateCommandList();

            m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile("Resources/Shaders/hello_triangle.glsl",
                                                                   Nexus::Graphics::VertexPositionTexCoordNormal::GetLayout());

            Nexus::Graphics::PipelineDescription pipelineDescription;
            pipelineDescription.RasterizerStateDescription.CullMode = Nexus::Graphics::CullMode::None;
            pipelineDescription.RasterizerStateDescription.FrontFace = Nexus::Graphics::FrontFace::CounterClockwise;
            pipelineDescription.Shader = m_Shader;

            m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDescription);

            std::vector<Nexus::Graphics::VertexPositionTexCoordNormal> vertices =
                {
                    {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}}, // bottom left
                    {{0.0f, 0.5f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},   // top left
                    {{0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},  // bottom right
                };

            Nexus::Graphics::BufferDescription vertexBufferDesc;
            vertexBufferDesc.Size = vertices.size() * sizeof(Nexus::Graphics::VertexPositionTexCoordNormal);
            vertexBufferDesc.Type = Nexus::Graphics::BufferType::Vertex;
            vertexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
            m_VertexBuffer = m_GraphicsDevice->CreateVertexBuffer(vertexBufferDesc, vertices.data(), Nexus::Graphics::VertexPositionTexCoordNormal::GetLayout());
        }

        virtual void Render(Nexus::Time time) override
        {
            Nexus::Graphics::Viewport vp;
            vp.X = 0;
            vp.Y = 0;
            vp.Width = m_Window->GetWindowSize().X;
            vp.Height = m_Window->GetWindowSize().Y;
            m_GraphicsDevice->SetViewport(vp);

            Nexus::Graphics::CommandListBeginInfo beginInfo{};
            beginInfo.ClearValue = {
                m_ClearColour.r,
                m_ClearColour.g,
                m_ClearColour.b,
                1.0f};

            m_CommandList->Begin(beginInfo);
            m_CommandList->SetPipeline(m_Pipeline);
            m_CommandList->SetVertexBuffer(m_VertexBuffer);

            auto vertexCount = m_VertexBuffer->GetDescription().Size / sizeof(Nexus::Graphics::VertexPositionTexCoordNormal);
            m_CommandList->DrawElements(0, vertexCount);
            m_CommandList->End();

            m_GraphicsDevice->SubmitCommandList(m_CommandList);
        }

        virtual void OnResize(Nexus::Point<int> size) override
        {
        }

        virtual void RenderUI() override
        {
        }

    private:
        Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
        Nexus::Ref<Nexus::Graphics::Shader> m_Shader;
        Nexus::Ref<Nexus::Graphics::Pipeline> m_Pipeline;
        Nexus::Ref<Nexus::Graphics::VertexBuffer> m_VertexBuffer;
        glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};
    };
}