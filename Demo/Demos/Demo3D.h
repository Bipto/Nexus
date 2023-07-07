#pragma once

#include "Demo.h"

#include "glm/gtx/transform.hpp"
#include "glm/gtx/quaternion.hpp"

namespace Demos
{
    struct alignas(16) VB_UNIFORM_CAMERA_DEMO_3D
    {
        glm::mat4 View;
        glm::mat4 Projection;
    };

    struct alignas(16) VB_UNIFORM_TRANSFORM_DEMO_3D
    {
        glm::mat4 Transform;
    };

    class Demo3D : public Demo
    {
    public:
        Demo3D(const std::string &name, Nexus::Application *app)
            : Demo(name, app)
        {
            m_CommandList = m_GraphicsDevice->CreateCommandList();

            Nexus::Graphics::VertexBufferLayout layout =
                {
                    {Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD", 0},
                    {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD", 1}};

            m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile("Resources/Shaders/3d.glsl", layout);

            Nexus::Graphics::PipelineDescription pipelineDescription;
            pipelineDescription.RasterizerStateDescription.CullMode = Nexus::Graphics::CullMode::Back;
            pipelineDescription.RasterizerStateDescription.FrontFace = Nexus::Graphics::FrontFace::CounterClockwise;
            pipelineDescription.Shader = m_Shader;

            m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDescription);

            Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
            m_Mesh = factory.CreateCube();

            m_Texture = m_GraphicsDevice->CreateTexture("Resources/Textures/raw_plank_wall_diff_1k.jpg");

            Nexus::Graphics::UniformResourceBinding cameraUniformBinding;
            cameraUniformBinding.Binding = 0;
            cameraUniformBinding.Name = "Camera";
            cameraUniformBinding.Size = sizeof(VB_UNIFORM_CAMERA_DEMO_3D);
            m_CameraUniformBuffer = m_GraphicsDevice->CreateUniformBuffer(cameraUniformBinding);
            m_CameraUniformBuffer->BindToShader(m_Shader);

            Nexus::Graphics::UniformResourceBinding transformUniformBinding;
            transformUniformBinding.Binding = 1;
            transformUniformBinding.Name = "Transform";
            transformUniformBinding.Size = sizeof(VB_UNIFORM_TRANSFORM_DEMO_3D);
            m_TransformUniformBuffer = m_GraphicsDevice->CreateUniformBuffer(transformUniformBinding);
            m_TransformUniformBuffer->BindToShader(m_Shader);
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

            m_TransformUniforms.Transform = glm::rotate(glm::mat4(1.0f), glm::radians((float)m_ElapsedTime.GetSeconds() * 100.0f), glm::vec3(0.0f, 1.0f, 1.0f));

            m_TransformUniformBuffer->SetData(&m_TransformUniforms, sizeof(m_TransformUniforms), 0);

            m_CameraUniforms.View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f));
            m_CameraUniforms.Projection = glm::perspectiveFov<float>(glm::radians(60.0f),
                                                                     m_Window->GetWindowSize().X,
                                                                     m_Window->GetWindowSize().Y,
                                                                     0.1f, 100.0f);

            m_CameraUniformBuffer->SetData(&m_CameraUniforms, sizeof(m_CameraUniforms), 0);

            m_CommandList->Begin(beginInfo);
            m_CommandList->SetPipeline(m_Pipeline);
            m_CommandList->SetVertexBuffer(m_Mesh.GetVertexBuffer());
            m_CommandList->SetIndexBuffer(m_Mesh.GetIndexBuffer());
            m_CommandList->DrawIndexed(m_Mesh.GetIndexBuffer()->GetIndexCount(), 0);
            m_CommandList->End();

            m_GraphicsDevice->SubmitCommandList(m_CommandList);

            m_ElapsedTime = m_ElapsedTime.GetNanoseconds() + time.GetNanoseconds();
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
        Nexus::Graphics::Mesh m_Mesh;
        Nexus::Ref<Nexus::Graphics::Texture> m_Texture;
        glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};

        VB_UNIFORM_CAMERA_DEMO_3D m_CameraUniforms;
        Nexus::Ref<Nexus::Graphics::UniformBuffer> m_CameraUniformBuffer;

        VB_UNIFORM_TRANSFORM_DEMO_3D m_TransformUniforms;
        Nexus::Ref<Nexus::Graphics::UniformBuffer> m_TransformUniformBuffer;

        Nexus::Time m_ElapsedTime = 0;
    };
}