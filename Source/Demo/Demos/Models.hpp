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
        ModelDemo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
            : Demo(name, app, imGuiRenderer)
        {
            m_CommandList = m_GraphicsDevice->CreateCommandList();

            m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile(Nexus::FileSystem::GetFilePathAbsolute("resources/shaders/models.glsl"));

            Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
            m_Model = factory.CreateFrom3DModelFile(Nexus::FileSystem::GetFilePathAbsolute("resources/models/survival_backpack_2/survival_backpack_2.fbx"));

            m_DiffuseMap = m_GraphicsDevice->CreateTexture(Nexus::FileSystem::GetFilePathAbsolute("resources/models/survival_backpack_2/1001_albedo.jpg"));
            m_NormalMap = m_GraphicsDevice->CreateTexture(Nexus::FileSystem::GetFilePathAbsolute("resources/models/survival_backpack_2/1001_normal.png"));
            m_SpecularMap = m_GraphicsDevice->CreateTexture(Nexus::FileSystem::GetFilePathAbsolute("resources/models/survival_backpack_2/specular.jpg"));

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

            Nexus::Graphics::SamplerSpecification samplerSpec{};
            m_Sampler = m_GraphicsDevice->CreateSampler(samplerSpec);
        }

        virtual ~ModelDemo()
        {
            delete m_Model;
        }

        virtual void Render(Nexus::Time time) override
        {
            m_CameraUniforms.View = m_Camera.GetView();
            m_CameraUniforms.Projection = m_Camera.GetProjection();
            m_CameraUniforms.CamPosition = m_Camera.GetPosition();
            m_CameraUniformBuffer->SetData(&m_CameraUniforms, sizeof(m_CameraUniforms));

            m_TransformUniforms.Transform = glm::translate(glm::mat4(1.0f), {0.0f, 0.0f, -10.0f});
            m_TransformUniformBuffer->SetData(&m_TransformUniforms, sizeof(m_TransformUniforms));

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

            m_CommandList->ClearColorTarget(0,
                                            {m_ClearColour.r,
                                             m_ClearColour.g,
                                             m_ClearColour.b, 1.0f});

            Nexus::Graphics::ClearDepthStencilValue value;
            m_CommandList->ClearDepthTarget(value);

            m_ResourceSet->WriteUniformBuffer(m_CameraUniformBuffer, "Camera");
            m_ResourceSet->WriteUniformBuffer(m_TransformUniformBuffer, "Transform");

            m_ResourceSet->WriteCombinedImageSampler(m_DiffuseMap, m_Sampler, "diffuseMapSampler");
            m_ResourceSet->WriteCombinedImageSampler(m_NormalMap, m_Sampler, "normalMapSampler");
            m_ResourceSet->WriteCombinedImageSampler(m_SpecularMap, m_Sampler, "specularMapSampler");

            m_CommandList->SetResourceSet(m_ResourceSet);

            auto &meshes = m_Model->GetMeshes();
            for (auto mesh : meshes)
            {
                m_CommandList->SetVertexBuffer(mesh->GetVertexBuffer(), 0);
                m_CommandList->SetIndexBuffer(mesh->GetIndexBuffer());

                auto indexCount = mesh->GetIndexBuffer()->GetDescription().Size / sizeof(unsigned int);
                m_CommandList->DrawIndexed(indexCount, 0, 0);
            }

            m_CommandList->End();

            m_GraphicsDevice->SubmitCommandList(m_CommandList);

            m_Camera.Update(
                m_Window->GetWindowSize().X,
                m_Window->GetWindowSize().Y,
                time);

            m_Rotation += 0.05f * time.GetMilliseconds();
        }

        virtual void OnResize(Nexus::Point<uint32_t> size) override
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
            pipelineDescription.Layouts = {Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout()};

            pipelineDescription.ResourceSetSpecification.UniformBuffers =
                {
                    {"Camera", 0, 0},
                    {"Transform", 0, 1}};

            pipelineDescription.ResourceSetSpecification.Textures =
                {
                    {"diffuseMapSampler", 1, 0},
                    {"normalMapSampler", 1, 1},
                    {"specularMapSampler", 1, 2}};

            pipelineDescription.Target = {m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()};

            m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDescription);
            m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
        }

    private:
        Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
        Nexus::Ref<Nexus::Graphics::Shader> m_Shader;
        Nexus::Ref<Nexus::Graphics::Pipeline> m_Pipeline;
        Nexus::Graphics::Model *m_Model;
        Nexus::Ref<Nexus::Graphics::Texture> m_DiffuseMap;
        Nexus::Ref<Nexus::Graphics::Texture> m_NormalMap;
        Nexus::Ref<Nexus::Graphics::Texture> m_SpecularMap;
        glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};

        Nexus::Ref<Nexus::Graphics::ResourceSet> m_ResourceSet;

        VB_UNIFORM_CAMERA_DEMO_MODELS m_CameraUniforms;
        Nexus::Ref<Nexus::Graphics::UniformBuffer> m_CameraUniformBuffer;

        VB_UNIFORM_TRANSFORM_DEMO_MODELS m_TransformUniforms;
        Nexus::Ref<Nexus::Graphics::UniformBuffer> m_TransformUniformBuffer;

        Nexus::Ref<Nexus::Graphics::Sampler> m_Sampler;

        Nexus::FirstPersonCamera m_Camera;

        float m_Rotation = 0.0f;
    };
}