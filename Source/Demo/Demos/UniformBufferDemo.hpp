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
            m_CommandList = m_GraphicsDevice->CreateCommandList();
            m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile(Nexus::FileSystem::GetFilePathAbsolute("resources/shaders/uniform_buffers.glsl"),
                                                                   Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout());

            CreatePipeline();

            Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
            m_Mesh = factory.CreateSprite();

            m_Texture = m_GraphicsDevice->CreateTexture(Nexus::FileSystem::GetFilePathAbsolute("resources/textures/brick.jpg"));

            Nexus::Graphics::SamplerSpecification samplerSpec{};
            m_Sampler = m_GraphicsDevice->CreateSampler(samplerSpec);
        }

        virtual ~UniformBufferDemo()
        {
            delete m_CommandList;
            delete m_Shader;
            delete m_Pipeline;
            delete m_Texture;
            delete m_ResourceSet;
            delete m_Mesh;
            delete m_TransformUniformBuffer;
        }

        virtual void Update(Nexus::Time time) override
        {
        }

        virtual void Render(Nexus::Time time) override
        {
            m_TransformUniforms.Transform = glm::translate(glm::mat4(1.0f), m_Position);

            void *buffer = m_TransformUniformBuffer->Map();
            memcpy(buffer, &m_TransformUniforms, sizeof(m_TransformUniforms));
            m_TransformUniformBuffer->Unmap();

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

            m_CommandList->ClearColorTarget(0, {m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, 1.0f});

            m_ResourceSet->WriteUniformBuffer(m_TransformUniformBuffer, "Transform");
            m_ResourceSet->WriteCombinedImageSampler(m_Texture, m_Sampler, "texSampler");
            m_CommandList->SetResourceSet(m_ResourceSet);

            m_CommandList->SetVertexBuffer(m_Mesh->GetVertexBuffer());
            m_CommandList->SetIndexBuffer(m_Mesh->GetIndexBuffer());

            auto indexCount = m_Mesh->GetIndexBuffer()->GetDescription().Size / sizeof(unsigned int);
            m_CommandList->DrawIndexed(indexCount, 0, 0);
            m_CommandList->End();

            m_GraphicsDevice->SubmitCommandList(m_CommandList);
        }

        virtual void RenderUI() override
        {
            ImGui::DragFloat2("Position", glm::value_ptr(m_Position), 0.1f, -1.0f, 1.0f);
        }

        virtual void OnResize(Nexus::Point<uint32_t> size) override
        {
        }

    private:
        void CreatePipeline()
        {
            Nexus::Graphics::PipelineDescription pipelineDescription;
            pipelineDescription.RasterizerStateDescription.CullMode = Nexus::Graphics::CullMode::Back;
            pipelineDescription.RasterizerStateDescription.FrontFace = Nexus::Graphics::FrontFace::CounterClockwise;
            pipelineDescription.Shader = m_Shader;

            Nexus::Graphics::BufferDescription transformUniformBufferDesc;
            transformUniformBufferDesc.Size = sizeof(VB_UNIFORM_TRANSFORM_UNIFORM_BUFFER_DEMO);
            transformUniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
            m_TransformUniformBuffer = m_GraphicsDevice->CreateUniformBuffer(transformUniformBufferDesc, nullptr);

            pipelineDescription.ResourceSetSpecification.UniformBuffers =
                {
                    {"Transform", 0, 0}};

            pipelineDescription.ResourceSetSpecification.Textures =
                {
                    {"texSampler", 1, 0}};

            pipelineDescription.Target = {m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()};

            m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDescription);
            m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
        }

    private:
        Nexus::Graphics::CommandList *m_CommandList;
        Nexus::Graphics::Shader *m_Shader;
        Nexus::Graphics::Pipeline *m_Pipeline;
        Nexus::Graphics::Texture *m_Texture;
        Nexus::Graphics::ResourceSet *m_ResourceSet;
        Nexus::Graphics::Mesh *m_Mesh;
        Nexus::Graphics::Sampler *m_Sampler;
        glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};

        glm::vec3 m_Position{0.0f, 0.0f, 0.0f};

        VB_UNIFORM_TRANSFORM_UNIFORM_BUFFER_DEMO m_TransformUniforms;
        Nexus::Graphics::UniformBuffer *m_TransformUniformBuffer;
    };
}