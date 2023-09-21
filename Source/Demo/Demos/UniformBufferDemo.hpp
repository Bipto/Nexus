#pragma once

#include "Demo.hpp"

namespace Demos
{
    struct alignas(16) VB_UNIFORM_TRANSFORM_UNIFORM_BUFFER_DEMO
    {
        glm::mat4 Transform;
    };

    class UniformBufferDemo : public Demo
    {
    public:
        UniformBufferDemo(const std::string &name, Nexus::Application *app)
            : Demo(name, app)
        {
            Nexus::Graphics::RenderPassSpecification spec;
            m_RenderPass = m_GraphicsDevice->CreateRenderPass(spec, m_GraphicsDevice->GetSwapchain());
            m_CommandList = m_GraphicsDevice->CreateCommandList();

            m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile("Resources/Shaders/uniform_buffers.glsl",
                                                                   Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout());

            CreatePipeline();

            Nexus::Graphics::MeshFactory factory(m_GraphicsDevice.get());
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

            m_TransformUniforms.Transform = glm::translate(glm::mat4(1.0f), m_Position);
            m_TransformUniformBuffer->SetData(&m_TransformUniforms, sizeof(m_TransformUniforms), 0);

            m_CommandList->Begin();
            m_CommandList->BeginRenderPass(m_RenderPass, beginInfo);
            m_CommandList->SetPipeline(m_Pipeline);

            m_ResourceSet->WriteUniformBuffer(m_TransformUniformBuffer, 0);
            m_ResourceSet->WriteTexture(m_Texture, 0);
            m_CommandList->SetResourceSet(m_ResourceSet);

            m_CommandList->SetVertexBuffer(m_Mesh->GetVertexBuffer());
            m_CommandList->SetIndexBuffer(m_Mesh->GetIndexBuffer());

            auto indexCount = m_Mesh->GetIndexBuffer()->GetDescription().Size / sizeof(unsigned int);
            m_CommandList->DrawIndexed(indexCount, 0);
            m_CommandList->EndRenderPass();
            m_CommandList->End();

            m_GraphicsDevice->SubmitCommandList(m_CommandList);
        }

        virtual void RenderUI() override
        {
            ImGui::DragFloat2("Position", glm::value_ptr(m_Position), 0.1f, -1.0f, 1.0f);
        }

        virtual void OnResize(Nexus::Point<int> size) override
        {
            CreatePipeline();
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

            Nexus::Graphics::BufferDescription transformUniformBufferDesc;
            transformUniformBufferDesc.Size = sizeof(VB_UNIFORM_TRANSFORM_UNIFORM_BUFFER_DEMO);
            transformUniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
            m_TransformUniformBuffer = m_GraphicsDevice->CreateUniformBuffer(transformUniformBufferDesc, nullptr);

            Nexus::Graphics::UniformResourceBinding transformUniformBinding;
            transformUniformBinding.Binding = 0;
            transformUniformBinding.Name = "Transform";
            transformUniformBinding.Buffer = m_TransformUniformBuffer;

            Nexus::Graphics::TextureResourceBinding textureBinding;
            textureBinding.Slot = 0;
            textureBinding.Name = "texSampler";

            Nexus::Graphics::ResourceSetSpecification resources;
            resources.UniformResourceBindings = {transformUniformBinding};
            resources.TextureBindings = {textureBinding};
            pipelineDescription.ResourceSetSpecification = resources;

            m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDescription);
            m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
        }

    private:
        Nexus::Graphics::RenderPass *m_RenderPass;
        Nexus::Graphics::CommandList *m_CommandList;
        Nexus::Graphics::Shader *m_Shader;
        Nexus::Graphics::Pipeline *m_Pipeline;
        Nexus::Graphics::Texture *m_Texture;
        Nexus::Graphics::ResourceSet *m_ResourceSet;
        Nexus::Graphics::Mesh *m_Mesh;
        glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};

        glm::vec3 m_Position{0.0f, 0.0f, 0.0f};

        VB_UNIFORM_TRANSFORM_UNIFORM_BUFFER_DEMO m_TransformUniforms;
        Nexus::Graphics::UniformBuffer *m_TransformUniformBuffer;
    };
}