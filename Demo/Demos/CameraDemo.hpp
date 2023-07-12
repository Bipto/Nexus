#pragma once

#include "Demo.hpp"

#include "glm/gtx/transform.hpp"
#include "glm/gtx/quaternion.hpp"

namespace Demos
{
    struct alignas(16) VB_UNIFORM_CAMERA_DEMO_CAMERA
    {
        glm::mat4 View;
        glm::mat4 Projection;
    };

    struct alignas(16) VB_UNIFORM_TRANSFORM_DEMO_CAMERA
    {
        glm::mat4 Transform;
    };

    class CameraDemo : public Demo
    {
    public:
        CameraDemo(const std::string &name, Nexus::Application *app)
            : Demo(name, app)
        {
            m_CommandList = m_GraphicsDevice->CreateCommandList();

            m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile("Resources/Shaders/3d.glsl",
                                                                   Nexus::Graphics::VertexPositionTexCoordNormal::GetLayout());

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
            cameraUniformBinding.Size = sizeof(VB_UNIFORM_CAMERA_DEMO_CAMERA);

            Nexus::Graphics::BufferDescription cameraUniformBufferDesc;
            cameraUniformBufferDesc.Size = sizeof(VB_UNIFORM_CAMERA_DEMO_CAMERA);
            cameraUniformBufferDesc.Type = Nexus::Graphics::BufferType::Uniform;
            cameraUniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;

            m_CameraUniformBuffer = m_GraphicsDevice->CreateDeviceBuffer(cameraUniformBufferDesc);
            m_Shader->BindUniformBuffer(m_CameraUniformBuffer, cameraUniformBinding);

            Nexus::Graphics::UniformResourceBinding transformUniformBinding;
            transformUniformBinding.Binding = 1;
            transformUniformBinding.Name = "Transform";
            transformUniformBinding.Size = sizeof(VB_UNIFORM_TRANSFORM_DEMO_CAMERA);

            Nexus::Graphics::BufferDescription transformUniformBufferDesc;
            transformUniformBufferDesc.Size = sizeof(VB_UNIFORM_TRANSFORM_DEMO_CAMERA);
            transformUniformBufferDesc.Type = Nexus::Graphics::BufferType::Uniform;
            transformUniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
            m_TransformUniformBuffer = m_GraphicsDevice->CreateDeviceBuffer(transformUniformBufferDesc);
            m_Shader->BindUniformBuffer(m_TransformUniformBuffer, transformUniformBinding);

            m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, -2.5f));
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

            m_TransformUniforms.Transform = glm::mat4(1.0f);
            m_TransformUniformBuffer->SetData(&m_TransformUniforms, sizeof(m_TransformUniforms), 0);

            m_CameraUniforms.View = m_Camera.GetView();
            m_CameraUniforms.Projection = m_Camera.GetProjection();

            m_CameraUniformBuffer->SetData(&m_CameraUniforms, sizeof(m_CameraUniforms), 0);

            m_CommandList->Begin(beginInfo);
            m_CommandList->SetPipeline(m_Pipeline);
            m_CommandList->SetVertexBuffer(m_Mesh.GetVertexBuffer());
            m_CommandList->SetIndexBuffer(m_Mesh.GetIndexBuffer());

            auto indexCount = m_Mesh.GetIndexBuffer()->GetDescription().Size / sizeof(unsigned int);
            m_CommandList->DrawIndexed(indexCount, 0);
            m_CommandList->End();

            m_GraphicsDevice->SubmitCommandList(m_CommandList);

            m_Camera.Update(
                m_Window->GetWindowSize().X,
                m_Window->GetWindowSize().Y,
                time);
        }

    private:
        Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
        Nexus::Ref<Nexus::Graphics::Shader> m_Shader;
        Nexus::Ref<Nexus::Graphics::Pipeline> m_Pipeline;
        Nexus::Graphics::Mesh m_Mesh;
        Nexus::Ref<Nexus::Graphics::Texture> m_Texture;
        glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};

        VB_UNIFORM_CAMERA_DEMO_CAMERA m_CameraUniforms;
        Nexus::Ref<Nexus::Graphics::DeviceBuffer> m_CameraUniformBuffer;

        VB_UNIFORM_TRANSFORM_DEMO_CAMERA m_TransformUniforms;
        Nexus::Ref<Nexus::Graphics::DeviceBuffer> m_TransformUniformBuffer;

        Nexus::FirstPersonCamera m_Camera;
    };
}