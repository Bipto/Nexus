#pragma once

#include "Demo.hpp"

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
            Nexus::Graphics::RenderPassSpecification spec;
            m_RenderPass = m_GraphicsDevice->CreateRenderPass(spec, m_GraphicsDevice->GetSwapchain());
            m_CommandList = m_GraphicsDevice->CreateCommandList();

            m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile("Resources/Shaders/3d.glsl",
                                                                   Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout());

            Nexus::Graphics::MeshFactory factory(m_GraphicsDevice.get());
            m_Mesh = factory.CreateCube();
            m_Texture = m_GraphicsDevice->CreateTexture("Resources/Textures/raw_plank_wall_diff_1k.jpg");

            Nexus::Graphics::BufferDescription cameraUniformBufferDesc;
            cameraUniformBufferDesc.Size = sizeof(VB_UNIFORM_CAMERA_DEMO_3D);
            cameraUniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
            m_CameraUniformBuffer = m_GraphicsDevice->CreateUniformBuffer(cameraUniformBufferDesc, nullptr);

            Nexus::Graphics::BufferDescription transformUniformBufferDesc;
            transformUniformBufferDesc.Size = sizeof(VB_UNIFORM_TRANSFORM_DEMO_3D);
            transformUniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
            m_TransformUniformBuffer = m_GraphicsDevice->CreateUniformBuffer(transformUniformBufferDesc, nullptr);

            CreatePipeline();
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

            m_TransformUniforms.Transform = glm::rotate(glm::mat4(1.0f), glm::radians((float)m_ElapsedTime.GetSeconds() * 100.0f), glm::vec3(0.0f, 1.0f, 1.0f));
            m_TransformUniformBuffer->SetData(&m_TransformUniforms, sizeof(m_TransformUniforms), 0);

            m_CameraUniforms.View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f));
            m_CameraUniforms.Projection = glm::perspectiveFov<float>(glm::radians(60.0f),
                                                                     m_Window->GetWindowSize().X,
                                                                     m_Window->GetWindowSize().Y,
                                                                     0.1f, 100.0f);
            m_CameraUniformBuffer->SetData(&m_CameraUniforms, sizeof(m_CameraUniforms), 0);

            m_CommandList->Begin();
            m_CommandList->BeginRenderPass(m_RenderPass, beginInfo);
            m_CommandList->SetPipeline(m_Pipeline);

            m_ResourceSet->WriteUniformBuffer(m_CameraUniformBuffer, 0);
            m_ResourceSet->WriteUniformBuffer(m_TransformUniformBuffer, 1);
            m_ResourceSet->WriteTexture(m_Texture, 0);
            m_CommandList->SetResourceSet(m_ResourceSet);

            m_CommandList->SetVertexBuffer(m_Mesh.GetVertexBuffer());
            m_CommandList->SetIndexBuffer(m_Mesh.GetIndexBuffer());

            auto indexCount = m_Mesh.GetIndexBuffer()->GetDescription().Size / sizeof(unsigned int);
            m_CommandList->DrawIndexed(indexCount, 0);
            m_CommandList->End();

            m_GraphicsDevice->SubmitCommandList(m_CommandList);

            m_ElapsedTime = m_ElapsedTime.GetNanoseconds() + time.GetNanoseconds();
        }

        virtual void RenderUI() override
        {
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
            m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
        }

    private:
        Nexus::Graphics::CommandList *m_CommandList;
        Nexus::Graphics::RenderPass *m_RenderPass;
        Nexus::Graphics::Shader *m_Shader;
        Nexus::Graphics::Pipeline *m_Pipeline;
        Nexus::Graphics::Mesh m_Mesh;
        Nexus::Graphics::Texture *m_Texture;
        glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};

        Nexus::Graphics::ResourceSet *m_ResourceSet;

        VB_UNIFORM_CAMERA_DEMO_3D m_CameraUniforms;
        Nexus::Graphics::UniformBuffer *m_CameraUniformBuffer;

        VB_UNIFORM_TRANSFORM_DEMO_3D m_TransformUniforms;
        Nexus::Graphics::UniformBuffer *m_TransformUniformBuffer;

        Nexus::Time m_ElapsedTime = 0;
    };
}