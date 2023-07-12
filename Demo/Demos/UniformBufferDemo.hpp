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

            m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile("Resources/Shaders/uniform_buffers.glsl",
                                                                   Nexus::Graphics::VertexPositionTexCoordNormal::GetLayout());

            Nexus::Graphics::PipelineDescription pipelineDescription;
            pipelineDescription.RasterizerStateDescription.CullMode = Nexus::Graphics::CullMode::None;
            pipelineDescription.RasterizerStateDescription.FrontFace = Nexus::Graphics::FrontFace::CounterClockwise;
            pipelineDescription.Shader = m_Shader;

            m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDescription);

            Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
            m_Mesh = factory.CreateSprite();

            m_Texture = m_GraphicsDevice->CreateTexture("Resources/Textures/brick.jpg");

            Nexus::Graphics::UniformResourceBinding transformUniformBinding;
            transformUniformBinding.Binding = 0;
            transformUniformBinding.Name = "Transform";
            transformUniformBinding.Size = sizeof(VB_UNIFORM_TRANSFORM_UNIFORM_BUFFER_DEMO);

            Nexus::Graphics::BufferDescription transformUniformBufferDesc;
            transformUniformBufferDesc.Size = sizeof(VB_UNIFORM_TRANSFORM_UNIFORM_BUFFER_DEMO);
            transformUniformBufferDesc.Type = Nexus::Graphics::BufferType::Uniform;
            transformUniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
            m_TransformUniformBuffer = m_GraphicsDevice->CreateDeviceBuffer(transformUniformBufferDesc);

            m_Shader->BindUniformBuffer(m_TransformUniformBuffer, transformUniformBinding);
        }

        virtual void Update(Nexus::Time time) override
        {
        }

        virtual void Render(Nexus::Time time) override
        {
            Nexus::Graphics::Viewport vp;
            vp.X = 0;
            vp.Y = 0;
            vp.Width = m_Window->GetWindowSize().X;
            vp.Height = m_Window->GetWindowSize().Y;
            m_GraphicsDevice->SetViewport(vp);

            Nexus::Graphics::TextureBinding textureBinding;
            textureBinding.Slot = 0;
            textureBinding.Name = "texSampler";
            m_Shader->SetTexture(m_Texture, textureBinding);

            Nexus::Graphics::CommandListBeginInfo beginInfo{};
            beginInfo.ClearValue = {
                m_ClearColour.r,
                m_ClearColour.g,
                m_ClearColour.b,
                1.0f};

            m_TransformUniforms.Transform = glm::translate(glm::mat4(1.0f), m_Position);
            m_TransformUniformBuffer->SetData(&m_TransformUniforms, sizeof(m_TransformUniforms), 0);

            m_CommandList->Begin(beginInfo);
            m_CommandList->SetPipeline(m_Pipeline);
            m_CommandList->SetVertexBuffer(m_Mesh.GetVertexBuffer());
            m_CommandList->SetIndexBuffer(m_Mesh.GetIndexBuffer());

            auto indexCount = m_Mesh.GetIndexBuffer()->GetDescription().Size / sizeof(unsigned int);
            m_CommandList->DrawIndexed(indexCount, 0);
            m_CommandList->End();

            m_GraphicsDevice->SubmitCommandList(m_CommandList);
        }

        virtual void OnResize(Nexus::Point<int> size) override
        {
        }

        virtual void RenderUI() override
        {
            ImGui::DragFloat2("Position", glm::value_ptr(m_Position), 0.1f, -1.0f, 1.0f);
        }

    private:
        Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
        Nexus::Ref<Nexus::Graphics::Shader> m_Shader;
        Nexus::Ref<Nexus::Graphics::Pipeline> m_Pipeline;
        Nexus::Graphics::Mesh m_Mesh;
        Nexus::Ref<Nexus::Graphics::Texture> m_Texture;
        glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};

        glm::vec3 m_Position{0.0f, 0.0f, 0.0f};

        VB_UNIFORM_TRANSFORM_UNIFORM_BUFFER_DEMO m_TransformUniforms;
        Nexus::Ref<Nexus::Graphics::DeviceBuffer> m_TransformUniformBuffer;
    };
}