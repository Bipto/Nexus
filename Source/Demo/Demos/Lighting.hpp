#pragma once

#include "Demo.hpp"

#include "glm/gtx/transform.hpp"
#include "glm/gtx/quaternion.hpp"

namespace Demos
{
    struct alignas(16) VB_UNIFORM_CAMERA_DEMO_LIGHTING
    {
        glm::mat4 View;
        glm::mat4 Projection;
        glm::vec3 CamPosition;
    };

    struct alignas(16) VB_UNIFORM_TRANSFORM_DEMO_LIGHTING
    {
        glm::mat4 Transform;
    };

    class LightingDemo : public Demo
    {
    public:
        LightingDemo(const std::string &name, Nexus::Application *app)
            : Demo(name, app)
        {
            Nexus::Graphics::RenderPassSpecification spec;
            m_RenderPass = m_GraphicsDevice->CreateRenderPass(spec, m_GraphicsDevice->GetSwapchain());
            m_CommandList = m_GraphicsDevice->CreateCommandList();

            m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile("Resources/Shaders/lighting.glsl",
                                                                   Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout());

            Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
            m_CubeMesh = factory.CreateCube();
            m_SpriteMesh = factory.CreateSprite();

            m_DiffuseMap = m_GraphicsDevice->CreateTexture("Resources/Textures/raw_plank_wall_diff_1k.jpg");
            m_NormalMap = m_GraphicsDevice->CreateTexture("Resources/Textures/raw_plank_wall_normal_1k.jpg");
            m_SpecularMap = m_GraphicsDevice->CreateTexture("Resources/Textures/raw_plank_wall_spec_1k.jpg");

            Nexus::Graphics::BufferDescription cameraUniformBufferDesc;
            cameraUniformBufferDesc.Size = sizeof(VB_UNIFORM_CAMERA_DEMO_LIGHTING);
            cameraUniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
            m_CameraUniformBuffer = m_GraphicsDevice->CreateUniformBuffer(cameraUniformBufferDesc, nullptr);

            Nexus::Graphics::BufferDescription transformUniformBufferDesc;
            transformUniformBufferDesc.Size = sizeof(VB_UNIFORM_TRANSFORM_DEMO_LIGHTING);
            transformUniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
            m_TransformUniformBuffer = m_GraphicsDevice->CreateUniformBuffer(transformUniformBufferDesc, nullptr);

            CreatePipeline();
            m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, -2.5f));
        }

        virtual void Render(Nexus::Time time) override
        {
            Nexus::Graphics::TextureResourceBinding diffuseMapBinding;
            diffuseMapBinding.Slot = 0;
            diffuseMapBinding.Name = "diffuseMapSampler";
            m_Shader->SetTexture(m_DiffuseMap, diffuseMapBinding);

            Nexus::Graphics::TextureResourceBinding normalMapBinding;
            normalMapBinding.Slot = 1;
            normalMapBinding.Name = "normalMapSampler";
            m_Shader->SetTexture(m_NormalMap, normalMapBinding);

            Nexus::Graphics::TextureResourceBinding specularMapBinding;
            specularMapBinding.Slot = 2;
            specularMapBinding.Name = "specularMapSampler";
            m_Shader->SetTexture(m_SpecularMap, specularMapBinding);

            Nexus::Graphics::RenderPassBeginInfo beginInfo{};
            beginInfo.ClearColorValue = {
                m_ClearColour.r,
                m_ClearColour.g,
                m_ClearColour.b,
                1.0f};

            m_TransformUniforms.Transform = glm::mat4(1.0f);
            m_TransformUniformBuffer->SetData(&m_TransformUniforms, sizeof(m_TransformUniforms), 0);

            m_CameraUniforms.View = m_Camera.GetView();
            m_CameraUniforms.Projection = m_Camera.GetProjection();
            m_CameraUniforms.CamPosition = m_Camera.GetPosition();
            m_CameraUniformBuffer->SetData(&m_CameraUniforms, sizeof(m_CameraUniforms), 0);

            m_CommandList->Begin();
            m_CommandList->SetPipeline(m_Pipeline);
            m_CommandList->BeginRenderPass(m_RenderPass, beginInfo);

            // draw cube
            {
                m_TransformUniforms.Transform = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, -5.0f));
                m_CommandList->UpdateUniformBuffer(m_TransformUniformBuffer, &m_TransformUniforms, sizeof(m_TransformUniforms), 0);
                m_CommandList->SetVertexBuffer(m_CubeMesh.GetVertexBuffer());
                m_CommandList->SetIndexBuffer(m_CubeMesh.GetIndexBuffer());

                auto indexCount = m_CubeMesh.GetIndexBuffer()->GetDescription().Size / sizeof(unsigned int);
                m_CommandList->DrawIndexed(indexCount, 0);
            }

            // draw sprite
            {
                m_TransformUniforms.Transform = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, -5.0f));
                m_CommandList->UpdateUniformBuffer(m_TransformUniformBuffer, &m_TransformUniforms, sizeof(m_TransformUniforms), 0);
                m_CommandList->SetVertexBuffer(m_SpriteMesh.GetVertexBuffer());
                m_CommandList->SetIndexBuffer(m_SpriteMesh.GetIndexBuffer());

                auto indexCount = m_SpriteMesh.GetIndexBuffer()->GetDescription().Size / sizeof(unsigned int);
                m_CommandList->DrawIndexed(indexCount, 0);
            }

            m_CommandList->SetPipeline(m_Pipeline);

            m_CommandList->EndRenderPass();
            m_CommandList->End();

            m_GraphicsDevice->SubmitCommandList(m_CommandList);

            m_Camera.Update(
                m_Window->GetWindowSize().X,
                m_Window->GetWindowSize().Y,
                time);

            m_Rotation += 0.05f * time.GetMilliseconds();
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

            Nexus::Graphics::UniformResourceBinding cameraUniformBinding;
            cameraUniformBinding.Binding = 0;
            cameraUniformBinding.Name = "Camera";
            cameraUniformBinding.Buffer = m_CameraUniformBuffer;

            Nexus::Graphics::UniformResourceBinding transformUniformBinding;
            transformUniformBinding.Binding = 1;
            transformUniformBinding.Name = "Transform";
            transformUniformBinding.Buffer = m_TransformUniformBuffer;

            Nexus::Graphics::ResourceSetSpecification resources;
            resources.UniformResourceBindings = {cameraUniformBinding, transformUniformBinding};
            pipelineDescription.ResourceSetSpecification = resources;

            m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDescription);
        }

    private:
        Nexus::Ref<Nexus::Graphics::RenderPass> m_RenderPass;
        Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
        Nexus::Ref<Nexus::Graphics::Shader> m_Shader;
        Nexus::Ref<Nexus::Graphics::Pipeline> m_Pipeline;
        Nexus::Graphics::Mesh m_CubeMesh;
        Nexus::Graphics::Mesh m_SpriteMesh;
        Nexus::Ref<Nexus::Graphics::Texture>
            m_DiffuseMap;
        Nexus::Ref<Nexus::Graphics::Texture> m_NormalMap;
        Nexus::Ref<Nexus::Graphics::Texture> m_SpecularMap;
        glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};

        VB_UNIFORM_CAMERA_DEMO_LIGHTING m_CameraUniforms;
        Nexus::Ref<Nexus::Graphics::UniformBuffer> m_CameraUniformBuffer;

        VB_UNIFORM_TRANSFORM_DEMO_LIGHTING m_TransformUniforms;
        Nexus::Ref<Nexus::Graphics::UniformBuffer> m_TransformUniformBuffer;

        Nexus::FirstPersonCamera m_Camera;

        float m_Rotation = 0.0f;
    };
}