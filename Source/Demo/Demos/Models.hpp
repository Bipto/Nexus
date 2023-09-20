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

            Nexus::Graphics::MeshFactory factory(m_GraphicsDevice.get());
            m_Model = factory.CreateFrom3DModelFile("Resources/Models/Survival_BackPack_2/Survival_BackPack_2.fbx");

            m_DiffuseMap = m_GraphicsDevice->CreateTexture("Resources/Models/Survival_BackPack_2/1001_albedo.jpg");
            m_NormalMap = m_GraphicsDevice->CreateTexture("Resources/Models/Survival_BackPack_2/1001_normal.png");
            m_SpecularMap = m_GraphicsDevice->CreateTexture("Resources/Models/Survival_BackPack_2/specular.jpg");

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
            Nexus::Graphics::RenderPassBeginInfo beginInfo{};
            beginInfo.ClearColorValue = {
                m_ClearColour.r,
                m_ClearColour.g,
                m_ClearColour.b,
                1.0f};

            m_CameraUniforms.View = m_Camera.GetView();
            m_CameraUniforms.Projection = m_Camera.GetProjection();
            m_CameraUniforms.CamPosition = m_Camera.GetPosition();
            m_CameraUniformBuffer->SetData(&m_CameraUniforms, sizeof(m_CameraUniforms), 0);

            m_CommandList->Begin();
            m_CommandList->SetPipeline(m_Pipeline);
            m_CommandList->BeginRenderPass(m_RenderPass, beginInfo);

            m_CommandList->SetPipeline(m_Pipeline);

            m_ResourceSet->WriteUniformBuffer(m_CameraUniformBuffer, 0);
            m_ResourceSet->WriteUniformBuffer(m_TransformUniformBuffer, 1);

            m_ResourceSet->WriteTexture(m_DiffuseMap, 0);
            m_ResourceSet->WriteTexture(m_NormalMap, 1);
            m_ResourceSet->WriteTexture(m_SpecularMap, 2);
            m_CommandList->SetResourceSet(m_ResourceSet);

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

            Nexus::Graphics::UniformResourceBinding cameraUniformBinding;
            cameraUniformBinding.Binding = 0;
            cameraUniformBinding.Name = "Camera";
            cameraUniformBinding.Buffer = m_CameraUniformBuffer;

            Nexus::Graphics::UniformResourceBinding transformUniformBinding;
            transformUniformBinding.Binding = 1;
            transformUniformBinding.Name = "Transform";
            transformUniformBinding.Buffer = m_TransformUniformBuffer;

            Nexus::Graphics::TextureResourceBinding diffuseMapBinding;
            diffuseMapBinding.Slot = 0;
            diffuseMapBinding.Name = "diffuseMapSampler";

            Nexus::Graphics::TextureResourceBinding normalMapBinding;
            normalMapBinding.Slot = 1;
            normalMapBinding.Name = "normalMapSampler";

            Nexus::Graphics::TextureResourceBinding specularMapBinding;
            specularMapBinding.Slot = 2;
            specularMapBinding.Name = "specularMapSampler";

            Nexus::Graphics::ResourceSetSpecification resources;
            resources.UniformResourceBindings = {cameraUniformBinding, transformUniformBinding};
            resources.TextureBindings = {diffuseMapBinding, normalMapBinding, specularMapBinding};
            pipelineDescription.ResourceSetSpecification = resources;

            m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDescription);
            m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
        }

    private:
        Nexus::Graphics::RenderPass *m_RenderPass;
        Nexus::Graphics::CommandList *m_CommandList;
        Nexus::Graphics::Shader *m_Shader;
        Nexus::Graphics::Pipeline *m_Pipeline;
        Nexus::Graphics::Model m_Model;
        Nexus::Graphics::Texture *m_DiffuseMap;
        Nexus::Graphics::Texture *m_NormalMap;
        Nexus::Graphics::Texture *m_SpecularMap;
        glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};

        Nexus::Graphics::ResourceSet *m_ResourceSet;

        VB_UNIFORM_CAMERA_DEMO_MODELS m_CameraUniforms;
        Nexus::Graphics::UniformBuffer *m_CameraUniformBuffer;

        VB_UNIFORM_TRANSFORM_DEMO_MODELS m_TransformUniforms;
        Nexus::Graphics::UniformBuffer *m_TransformUniformBuffer;

        Nexus::FirstPersonCamera m_Camera;

        float m_Rotation = 0.0f;
    };
}