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
            m_CommandList = m_GraphicsDevice->CreateCommandList();

            m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile(Nexus::FileSystem::GetFilePathAbsolute("resources/shaders/lighting.glsl"),
                                                                   Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout());

            Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
            m_CubeMesh = factory.CreateCube();

            m_DiffuseMap = m_GraphicsDevice->CreateTexture(Nexus::FileSystem::GetFilePathAbsolute("resources/textures/raw_plank_wall_diff_1k.jpg"));
            m_NormalMap = m_GraphicsDevice->CreateTexture(Nexus::FileSystem::GetFilePathAbsolute("resources/textures/raw_plank_wall_normal_1k.jpg"));
            m_SpecularMap = m_GraphicsDevice->CreateTexture(Nexus::FileSystem::GetFilePathAbsolute("resources/textures/raw_plank_wall_spec_1k.jpg"));

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

        virtual ~LightingDemo()
        {
            delete m_CommandList;
            delete m_Shader;
            delete m_Pipeline;

            delete m_CubeMesh;

            delete m_ResourceSet;
            delete m_DiffuseMap;
            delete m_NormalMap;
            delete m_SpecularMap;

            delete m_CameraUniformBuffer;
            delete m_TransformUniformBuffer;
        }

        virtual void Render(Nexus::Time time) override
        {
            m_TransformUniforms.Transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), {1.0f, 1.0f, 0.0f});

            void *buffer = m_TransformUniformBuffer->Map();
            memcpy(buffer, &m_TransformUniforms, sizeof(m_TransformUniforms));
            m_TransformUniformBuffer->Unmap();

            m_Rotation += time.GetSeconds();
            m_CameraUniforms.View = m_Camera.GetView();
            m_CameraUniforms.Projection = m_Camera.GetProjection();
            m_CameraUniforms.CamPosition = m_Camera.GetPosition();

            buffer = m_CameraUniformBuffer->Map();
            memcpy(buffer, &m_CameraUniforms, sizeof(m_CameraUniforms));
            m_CameraUniformBuffer->Unmap();

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
                m_ResourceSet->WriteUniformBuffer(m_CameraUniformBuffer, 0);
                m_ResourceSet->WriteUniformBuffer(m_TransformUniformBuffer, 1);

                m_ResourceSet->WriteTexture(m_DiffuseMap, 0);
                m_ResourceSet->WriteTexture(m_NormalMap, 1);
                m_ResourceSet->WriteTexture(m_SpecularMap, 2);
                m_CommandList->SetResourceSet(m_ResourceSet);
            }

            // draw cube
            {
                m_CommandList->SetVertexBuffer(m_CubeMesh->GetVertexBuffer());
                m_CommandList->SetIndexBuffer(m_CubeMesh->GetIndexBuffer());

                auto indexCount = m_CubeMesh->GetIndexBuffer()->GetDescription().Size / sizeof(unsigned int);
                m_CommandList->DrawIndexed(indexCount, 0, 0);
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

            Nexus::Graphics::UniformResourceBinding cameraUniformBinding;
            cameraUniformBinding.Binding = 0;
            cameraUniformBinding.Name = "Camera";

            Nexus::Graphics::UniformResourceBinding transformUniformBinding;
            transformUniformBinding.Binding = 1;
            transformUniformBinding.Name = "Transform";

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

            pipelineDescription.Target = {m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()};

            m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDescription);
            m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
        }

    private:
        Nexus::Graphics::CommandList *m_CommandList;
        Nexus::Graphics::Shader *m_Shader;
        Nexus::Graphics::Pipeline *m_Pipeline;
        Nexus::Graphics::Mesh *m_CubeMesh;

        Nexus::Graphics::ResourceSet *m_ResourceSet;
        Nexus::Graphics::Texture *m_DiffuseMap;
        Nexus::Graphics::Texture *m_NormalMap;
        Nexus::Graphics::Texture *m_SpecularMap;
        glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};

        VB_UNIFORM_CAMERA_DEMO_LIGHTING m_CameraUniforms;
        Nexus::Graphics::UniformBuffer *m_CameraUniformBuffer;

        VB_UNIFORM_TRANSFORM_DEMO_LIGHTING m_TransformUniforms;
        Nexus::Graphics::UniformBuffer *m_TransformUniformBuffer;

        Nexus::FirstPersonCamera m_Camera;

        float m_Rotation = 0.0f;
    };
}