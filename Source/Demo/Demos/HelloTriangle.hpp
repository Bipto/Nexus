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
            Nexus::Graphics::RenderPassSpecification spec;
            m_RenderPass = m_GraphicsDevice->CreateRenderPass(spec, app->GetPrimaryWindow()->GetSwapchain());
            m_CommandList = m_GraphicsDevice->CreateCommandList();

            m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile(Nexus::FileSystem::GetFilePathAbsolute("resources/shaders/hello_triangle.glsl"),
                                                                   Nexus::Graphics::VertexPositionTexCoordNormal::GetLayout());

            std::vector<Nexus::Graphics::VertexPositionTexCoordNormal> vertices =
                {
                    {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}}, // bottom left
                    {{0.0f, 0.5f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},   // top left
                    {{0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},  // bottom right
                };

            CreatePipeline();

            Nexus::Graphics::BufferDescription vertexBufferDesc;
            vertexBufferDesc.Size = vertices.size() * sizeof(Nexus::Graphics::VertexPositionTexCoordNormal);
            vertexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
            m_VertexBuffer = m_GraphicsDevice->CreateVertexBuffer(vertexBufferDesc, vertices.data(), Nexus::Graphics::VertexPositionTexCoordNormal::GetLayout());
        }

        virtual ~HelloTriangleDemo()
        {
            delete m_CommandList;
            delete m_RenderPass;
            delete m_Shader;
            delete m_Pipeline;
            delete m_VertexBuffer;
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
            m_CommandList->SetVertexBuffer(m_VertexBuffer);

            auto vertexCount = m_VertexBuffer->GetDescription().Size / sizeof(Nexus::Graphics::VertexPositionTexCoordNormal);
            m_CommandList->DrawElements(0, vertexCount);
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
            pipelineDescription.RasterizerStateDescription.CullMode = Nexus::Graphics::CullMode::None;
            pipelineDescription.RasterizerStateDescription.FrontFace = Nexus::Graphics::FrontFace::CounterClockwise;
            pipelineDescription.Shader = m_Shader;
            pipelineDescription.RenderPass = m_RenderPass;

            pipelineDescription.Viewport = {
                0, 0, m_Window->GetWindowSize().X, m_Window->GetWindowSize().Y};
            pipelineDescription.RasterizerStateDescription.ScissorRectangle = {
                0, 0, m_Window->GetWindowSize().X, m_Window->GetWindowSize().Y};

            m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDescription);
        }

    private:
        Nexus::Graphics::CommandList *m_CommandList;
        Nexus::Graphics::RenderPass *m_RenderPass;
        Nexus::Graphics::Shader *m_Shader;
        Nexus::Graphics::Pipeline *m_Pipeline;
        Nexus::Graphics::VertexBuffer *m_VertexBuffer;
        glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};
    };
}