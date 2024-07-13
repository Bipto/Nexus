#pragma once

#include "Demo.hpp"

#include "glm/gtx/transform.hpp"
#include "glm/gtx/quaternion.hpp"

namespace Demos
{
    struct alignas(16) VB_UNIFORM_CAMERA_DEMO_INSTANCING
    {
        glm::mat4 View;
        glm::mat4 Projection;
        glm::vec3 CamPosition;
    };

    class InstancingDemo : public Demo
    {
    public:
        InstancingDemo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
            : Demo(name, app, imGuiRenderer)
        {
            m_CommandList = m_GraphicsDevice->CreateCommandList();

            Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
            m_CubeMesh = factory.CreateCube();

            m_DiffuseMap = m_GraphicsDevice->CreateTexture(Nexus::FileSystem::GetFilePathAbsolute("resources/textures/raw_plank_wall_diff_1k.jpg"), true);
            m_NormalMap = m_GraphicsDevice->CreateTexture(Nexus::FileSystem::GetFilePathAbsolute("resources/textures/raw_plank_wall_normal_1k.jpg"), true);
            m_SpecularMap = m_GraphicsDevice->CreateTexture(Nexus::FileSystem::GetFilePathAbsolute("resources/textures/raw_plank_wall_spec_1k.jpg"), true);

            Nexus::Graphics::BufferDescription cameraUniformBufferDesc;
            cameraUniformBufferDesc.Size = sizeof(VB_UNIFORM_CAMERA_DEMO_INSTANCING);
            cameraUniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
            m_CameraUniformBuffer = m_GraphicsDevice->CreateUniformBuffer(cameraUniformBufferDesc, nullptr);

            Nexus::Graphics::BufferDescription vertexBufferDescription;
            vertexBufferDescription.Size = m_InstanceCount * sizeof(glm::mat4);
            vertexBufferDescription.Usage = Nexus::Graphics::BufferUsage::Dynamic;
            m_InstanceBuffer = m_GraphicsDevice->CreateVertexBuffer(vertexBufferDescription, nullptr);

            std::vector<glm::mat4> mvps(m_InstanceCount);
            for (uint32_t i = 0; i < m_InstanceCount; i++)
            {
                mvps[i] = glm::translate(glm::mat4(1.0f), glm::vec3(i * 2.0f, 0.0f, -5.0f));
            }
            m_InstanceBuffer->SetData(mvps.data(), mvps.size() * sizeof(glm::mat4), 0);

            CreatePipeline();
            m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, 2.5f));

            Nexus::Graphics::SamplerSpecification samplerSpec{};
            m_Sampler = m_GraphicsDevice->CreateSampler(samplerSpec);
        }

        virtual ~InstancingDemo()
        {
        }

        virtual void Render(Nexus::Time time) override
        {
            m_Rotation += time.GetSeconds();
            m_CameraUniforms.View = m_Camera.GetView();
            m_CameraUniforms.Projection = m_Camera.GetProjection();
            m_CameraUniforms.CamPosition = m_Camera.GetPosition();
            m_CameraUniformBuffer->SetData(&m_CameraUniforms, sizeof(m_CameraUniforms));

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
            m_CommandList->ClearColorTarget(0, {m_ClearColour.r,
                                                m_ClearColour.g,
                                                m_ClearColour.b,
                                                1.0f});
            Nexus::Graphics::ClearDepthStencilValue clearValue;
            m_CommandList->ClearDepthTarget(clearValue);

            // upload resources
            {
                m_ResourceSet->WriteUniformBuffer(m_CameraUniformBuffer, "Camera");
                m_ResourceSet->WriteCombinedImageSampler(m_DiffuseMap, m_Sampler, "diffuseMapSampler");
                m_ResourceSet->WriteCombinedImageSampler(m_NormalMap, m_Sampler, "normalMapSampler");
                m_ResourceSet->WriteCombinedImageSampler(m_SpecularMap, m_Sampler, "specularMapSampler");

                m_CommandList->SetResourceSet(m_ResourceSet);
            }

            // draw cube
            {
                m_CommandList->SetVertexBuffer(m_CubeMesh->GetVertexBuffer(), 0);
                m_CommandList->SetVertexBuffer(m_InstanceBuffer, 1);
                m_CommandList->SetIndexBuffer(m_CubeMesh->GetIndexBuffer());

                auto indexCount = m_CubeMesh->GetIndexBuffer()->GetDescription().Size / sizeof(unsigned int);
                // m_CommandList->DrawIndexed(indexCount, 0, 0);
                m_CommandList->DrawInstancedIndexed(indexCount, m_InstanceCount, 0, 0, 0);
            }

            m_CommandList->SetPipeline(m_Pipeline);

            m_CommandList->End();

            m_GraphicsDevice->SubmitCommandList(m_CommandList);

            m_Camera.Update(
                m_Window->GetWindowSize().X,
                m_Window->GetWindowSize().Y,
                time);

            m_Rotation += 0.05f * time.GetMilliseconds();
        }

        virtual void OnResize(Nexus::Point2D<uint32_t> size) override
        {
        }

    private:
        void CreatePipeline()
        {
            Nexus::Graphics::PipelineDescription pipelineDescription;
            pipelineDescription.RasterizerStateDesc.TriangleCullMode = Nexus::Graphics::CullMode::Back;
            pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::Clockwise;

            pipelineDescription.VertexModule = m_GraphicsDevice->CreateShaderModuleFromSpirvFile("resources/shaders/instancing.vert.glsl", Nexus::Graphics::ShaderStage::Vertex);
            pipelineDescription.FragmentModule = m_GraphicsDevice->CreateShaderModuleFromSpirvFile("resources/shaders/instancing.frag.glsl", Nexus::Graphics::ShaderStage::Fragment);

            pipelineDescription.ResourceSetSpec.UniformBuffers =
                {
                    {"Camera", 0, 0}};

            pipelineDescription.ResourceSetSpec.SampledImages =
                {
                    {"diffuseMapSampler", 1, 0},
                    {"normalMapSampler", 1, 1},
                    {"specularMapSampler", 1, 2}};

            Nexus::Graphics::VertexBufferLayout vertexLayout =
                {
                    {Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"},
                    {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"},
                    {Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"},
                    {Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"},
                    {Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"}};

            Nexus::Graphics::VertexBufferLayout instanceLayout =
                {
                    {Nexus::Graphics::ShaderDataType::Float4, "TEXCOORD"},
                    {Nexus::Graphics::ShaderDataType::Float4, "TEXCOORD"},
                    {Nexus::Graphics::ShaderDataType::Float4, "TEXCOORD"},
                    {Nexus::Graphics::ShaderDataType::Float4, "TEXCOORD"}};
            instanceLayout.SetInstanceStepRate(1);

            pipelineDescription.Layouts = {vertexLayout, instanceLayout};

            pipelineDescription.DepthStencilDesc.EnableDepthTest = true;
            pipelineDescription.DepthStencilDesc.EnableDepthWrite = true;
            pipelineDescription.DepthStencilDesc.DepthComparisonFunction = Nexus::Graphics::ComparisonFunction::Less;

            pipelineDescription.Target = {m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()};

            m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDescription);
            m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
        }

    private:
        Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
        Nexus::Ref<Nexus::Graphics::Pipeline> m_Pipeline;
        Nexus::Ref<Nexus::Graphics::Mesh> m_CubeMesh;
        Nexus::Ref<Nexus::Graphics::VertexBuffer> m_InstanceBuffer;

        Nexus::Ref<Nexus::Graphics::ResourceSet> m_ResourceSet;
        Nexus::Ref<Nexus::Graphics::Texture> m_DiffuseMap;
        Nexus::Ref<Nexus::Graphics::Texture> m_NormalMap;
        Nexus::Ref<Nexus::Graphics::Texture> m_SpecularMap;
        glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};

        VB_UNIFORM_CAMERA_DEMO_INSTANCING m_CameraUniforms;
        Nexus::Ref<Nexus::Graphics::UniformBuffer> m_CameraUniformBuffer;

        Nexus::Ref<Nexus::Graphics::Sampler> m_Sampler;

        Nexus::FirstPersonCamera m_Camera;

        const uint32_t m_InstanceCount = 10;

        float m_Rotation = 0.0f;
    };
}