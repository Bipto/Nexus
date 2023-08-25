#pragma once

#include "Demo.hpp"

#include "glm/gtx/transform.hpp"
#include "glm/gtx/quaternion.hpp"

namespace Demos
{
    struct alignas(16) VB_UNIFORM_CAMERA_DEMO_MODELS
    {
        glm::mat4 View;
        glm::mat4 Projection;
        glm::vec3 CamPosition;
    };

    struct alignas(16) VB_UNIFORM_TRANSFORM_DEMO_MODELS
    {
        glm::mat4 Transform;
    };

    class ModelDemo : public Demo
    {
    public:
        ModelDemo(const std::string &name, Nexus::Application *app)
            : Demo(name, app)
        {
            Nexus::Graphics::RenderPassSpecification spec;
            m_RenderPass = m_GraphicsDevice->CreateRenderPass(spec, m_GraphicsDevice->GetSwapchain());
            m_CommandList = m_GraphicsDevice->CreateCommandList();

            m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile("Resources/Shaders/models.glsl",
                                                                   Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout());

            CreatePipeline();

            Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
            m_Model = factory.CreateFrom3DModelFile("Resources/Models/Survival_BackPack_2/Survival_BackPack_2.fbx");

            m_DiffuseMap = m_GraphicsDevice->CreateTexture("Resources/Models/Survival_BackPack_2/1001_albedo.jpg");
            m_NormalMap = m_GraphicsDevice->CreateTexture("Resources/Models/Survival_BackPack_2/1001_normal.png");
            m_SpecularMap = m_GraphicsDevice->CreateTexture("Resources/Models/Survival_BackPack_2/specular.jpg");

            Nexus::Graphics::UniformResourceBinding cameraUniformBinding;
            cameraUniformBinding.Binding = 0;
            cameraUniformBinding.Name = "Camera";
            cameraUniformBinding.Size = sizeof(VB_UNIFORM_CAMERA_DEMO_LIGHTING);

            Nexus::Graphics::BufferDescription cameraUniformBufferDesc;
            cameraUniformBufferDesc.Size = sizeof(VB_UNIFORM_CAMERA_DEMO_LIGHTING);
            cameraUniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
            m_CameraUniformBuffer = m_GraphicsDevice->CreateUniformBuffer(cameraUniformBufferDesc, nullptr);
            m_Shader->BindUniformBuffer(m_CameraUniformBuffer, cameraUniformBinding);

            Nexus::Graphics::UniformResourceBinding transformUniformBinding;
            transformUniformBinding.Binding = 1;
            transformUniformBinding.Name = "Transform";
            transformUniformBinding.Size = sizeof(VB_UNIFORM_TRANSFORM_DEMO_LIGHTING);

            Nexus::Graphics::BufferDescription transformUniformBufferDesc;
            transformUniformBufferDesc.Size = sizeof(VB_UNIFORM_TRANSFORM_DEMO_LIGHTING);
            transformUniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
            m_TransformUniformBuffer = m_GraphicsDevice->CreateUniformBuffer(transformUniformBufferDesc, nullptr);
            m_Shader->BindUniformBuffer(m_TransformUniformBuffer, transformUniformBinding);

            m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, -2.5f));
        }

        virtual void Render(Nexus::Time time) override
        {
            Nexus::Graphics::TextureBinding diffuseMapBinding;
            diffuseMapBinding.Slot = 0;
            diffuseMapBinding.Name = "diffuseMapSampler";
            m_Shader->SetTexture(m_DiffuseMap, diffuseMapBinding);

            Nexus::Graphics::TextureBinding normalMapBinding;
            normalMapBinding.Slot = 1;
            normalMapBinding.Name = "normalMapSampler";
            m_Shader->SetTexture(m_NormalMap, normalMapBinding);

            Nexus::Graphics::TextureBinding specularMapBinding;
            specularMapBinding.Slot = 2;
            specularMapBinding.Name = "specularMapSampler";
            m_Shader->SetTexture(m_SpecularMap, specularMapBinding);

            Nexus::Graphics::RenderPassBeginInfo beginInfo{};
            beginInfo.ClearColorValue = {
                m_ClearColour.r,
                m_ClearColour.g,
                m_ClearColour.b,
                1.0f};

            m_CameraUniforms.View = m_Camera.GetView();
            m_CameraUniforms.Projection = m_Camera.GetProjection();
            m_CameraUniforms.CamPosition = m_Camera.GetPosition();

            void *buffer = m_CameraUniformBuffer->Map(Nexus::Graphics::MapMode::Write);
            memcpy(buffer, &m_CameraUniforms, sizeof(m_CameraUniforms));
            m_CameraUniformBuffer->Unmap();

            m_CommandList->Begin();
            m_CommandList->SetPipeline(m_Pipeline);
            m_CommandList->BeginRenderPass(m_RenderPass, beginInfo);

            m_CommandList->SetPipeline(m_Pipeline);
            m_TransformUniforms.Transform = glm::translate(glm::mat4(1.0f), {0.0f, 0.0f, -10.0f});
            m_CommandList->UpdateUniformBuffer(m_TransformUniformBuffer, &m_TransformUniforms, sizeof(m_TransformUniforms), 0);

            auto &meshes = m_Model.GetMeshes();
            for (auto mesh : meshes)
            {
                m_CommandList->SetVertexBuffer(mesh.GetVertexBuffer());
                m_CommandList->SetIndexBuffer(mesh.GetIndexBuffer());

                auto indexCount = mesh.GetIndexBuffer()->GetDescription().Size / sizeof(unsigned int);
                m_CommandList->DrawIndexed(indexCount, 0);
            }

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
            pipelineDescription.DepthStencilDescription.EnableDepthTest = true;
            pipelineDescription.DepthStencilDescription.EnableDepthWrite = true;
            pipelineDescription.DepthStencilDescription.DepthComparisonFunction = Nexus::Graphics::ComparisonFunction::Less;
            pipelineDescription.Shader = m_Shader;
            pipelineDescription.Viewport = {
                0, 0, m_Window->GetWindowSize().X, m_Window->GetWindowSize().Y};

            m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDescription);
        }

    private:
        Nexus::Ref<Nexus::Graphics::RenderPass> m_RenderPass;
        Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
        Nexus::Ref<Nexus::Graphics::Shader> m_Shader;
        Nexus::Ref<Nexus::Graphics::Pipeline> m_Pipeline;
        Nexus::Graphics::Model m_Model;
        Nexus::Ref<Nexus::Graphics::Texture> m_DiffuseMap;
        Nexus::Ref<Nexus::Graphics::Texture> m_NormalMap;
        Nexus::Ref<Nexus::Graphics::Texture> m_SpecularMap;
        glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};

        VB_UNIFORM_CAMERA_DEMO_MODELS m_CameraUniforms;
        Nexus::Ref<Nexus::Graphics::UniformBuffer> m_CameraUniformBuffer;

        VB_UNIFORM_TRANSFORM_DEMO_MODELS m_TransformUniforms;
        Nexus::Ref<Nexus::Graphics::UniformBuffer> m_TransformUniformBuffer;

        Nexus::FirstPersonCamera m_Camera;

        float m_Rotation = 0.0f;
    };
}