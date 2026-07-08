#pragma once

#include "Demo.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/transform.hpp"

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
        ModelDemo(
            const std::string &name, Nexus::Application *app,
            Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer,
            Nexus::Graphics::CommandQueueHandle commandQueue
        )
            : Demo(name, app, imGuiRenderer, commandQueue)
        {
            auto [width, height] = m_Window->GetWindowSizeInPixels();
            m_Camera = Nexus::FirstPersonCamera(
                m_GraphicsDevice, width, height, glm::vec3(0.0f, 0.5f, 2.0f)
            );
        }

        virtual ~ModelDemo()
        {
        }

        virtual void Load() override
        {
            m_CommandList = m_CommandQueue->CreateCommandList();
            Nexus::Graphics::MeshFactory factory(m_GraphicsDevice, m_CommandQueue);
            m_Model = factory.CreateFrom3DModelFile(
                Nexus::FileSystem::GetFilePathAbsolute(
                    "resources/demo/models/The Boss/The Boss.dae"
                )
            );

            Nexus::Graphics::DeviceBufferDescription cameraUniformBufferDesc = {};
            cameraUniformBufferDesc.Access =
                Nexus::Graphics::BufferMemoryAccess::Upload;
            cameraUniformBufferDesc.Usage = Nexus::Graphics::BufferUsage_Uniform;
            cameraUniformBufferDesc.StrideInBytes =
                sizeof(VB_UNIFORM_CAMERA_DEMO_LIGHTING);
            cameraUniformBufferDesc.SizeInBytes =
                sizeof(VB_UNIFORM_CAMERA_DEMO_LIGHTING);
            m_CameraUniformBuffer =
                m_GraphicsDevice->CreateDeviceBuffer(cameraUniformBufferDesc);

            Nexus::Graphics::DeviceBufferDescription transformUniformBufferDesc = {};
            transformUniformBufferDesc.Access =
                Nexus::Graphics::BufferMemoryAccess::Upload;
            transformUniformBufferDesc.Usage = Nexus::Graphics::BufferUsage_Uniform;
            transformUniformBufferDesc.StrideInBytes =
                sizeof(VB_UNIFORM_TRANSFORM_DEMO_LIGHTING);
            transformUniformBufferDesc.SizeInBytes =
                sizeof(VB_UNIFORM_TRANSFORM_DEMO_LIGHTING);
            m_TransformUniformBuffer =
                m_GraphicsDevice->CreateDeviceBuffer(transformUniformBufferDesc);

            CreatePipeline();
            m_Camera.SetPosition(glm::vec3(0.0f, 0.5f, -1.5f));

            Nexus::Graphics::SamplerDescription samplerSpec{};
            m_Sampler = m_GraphicsDevice->CreateSampler(samplerSpec);
        }

        virtual void Render(Nexus::TimeSpan time) override
        {
            auto [width, height] =
                Nexus::GetApplication()->GetPrimaryWindow()->GetWindowSize();
            m_Camera.Update(width, height, time);

            // handle camera movement
            if (m_CameraActive)
            {
                Nexus::IWindow *window = Nexus::GetApplication()->GetPrimaryWindow();
                auto [windowWidth, windowHeight] = window->GetWindowSize();
                window->WarpMouse(
                    static_cast<float>(windowWidth) / 2.0f,
                    static_cast<float>(windowHeight) / 2.0f
                );

                glm::vec3 movement = {0.0f, 0.0f, 0.0f};
                float cameraSpeed = 2.0f * time.GetSeconds<float>();

                if (Nexus::Input::IsKeyDown(Nexus::ScanCode::LeftShift) ||
                    Nexus::Input::IsKeyDown(Nexus::ScanCode::RightShift))
                {
                    cameraSpeed *= 2.0f;
                }

                if (Nexus::Input::IsKeyDown(Nexus::ScanCode::W))
                {
                    movement.z += cameraSpeed;
                }

                if (Nexus::Input::IsKeyDown(Nexus::ScanCode::S))
                {
                    movement.z -= cameraSpeed;
                }

                if (Nexus::Input::IsKeyDown(Nexus::ScanCode::A))
                {
                    movement.x += cameraSpeed;
                }

                if (Nexus::Input::IsKeyDown(Nexus::ScanCode::D))
                {
                    movement.x -= cameraSpeed;
                }

                m_Camera.Translate(movement);
            }

            m_CameraUniforms.View = m_Camera.GetView();
            m_CameraUniforms.Projection = m_Camera.GetProjection();
            m_CameraUniforms.CamPosition = m_Camera.GetPosition();
            m_CameraUniformBuffer->SetData(
                &m_CameraUniforms, 0, sizeof(m_CameraUniforms)
            );

            m_TransformUniforms.Transform =
                glm::rotate(
                    glm::mat4(1.0f), glm::radians(m_Rotation),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                ) *
                glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
            m_TransformUniformBuffer->SetData(
                &m_TransformUniforms, 0, sizeof(m_TransformUniforms)
            );

            m_CommandList->Begin();
            m_CommandList->SetPipeline(m_Pipeline);

            Nexus::Graphics::SwapchainHandle swapchain =
                Nexus::GetApplication()->GetPrimarySwapchain();
            Nexus::Graphics::FramebufferHandle framebuffer =
                swapchain->GetCurrentFramebuffer();
            m_CommandList->SetFramebuffer(framebuffer);

            Nexus::Graphics::Viewport vp;
            vp.X = 0;
            vp.Y = 0;
            vp.Width = width;
            vp.Height = height;
            vp.MinDepth = 0.0f;
            vp.MaxDepth = 1.0f;
            m_CommandList->SetViewport(vp);

            Nexus::Graphics::Scissor scissor;
            scissor.X = 0;
            scissor.Y = 0;
            scissor.Width = width;
            scissor.Height = height;
            m_CommandList->SetScissor(scissor);

            m_CommandList->ClearColourTarget(
                0, {m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, 1.0f}
            );

            Nexus::Graphics::ClearDepthStencilValue value;
            m_CommandList->ClearDepthTarget(value);

            const auto &meshes = m_Model->GetMeshes();

            for (size_t i = 0; i < meshes.size(); i++)
            {
                const auto &mesh = meshes[i];
                const auto &mat = mesh->GetMaterial();
                auto resourceSet = m_ResourceSets[i];

                Nexus::Graphics::UniformBufferView cameraUniformBufferView = {};
                cameraUniformBufferView.BufferHandle = m_CameraUniformBuffer;
                cameraUniformBufferView.Offset = 0;
                cameraUniformBufferView.Size =
                    m_CameraUniformBuffer->GetDescription().SizeInBytes;
                resourceSet->WriteUniformBuffer(cameraUniformBufferView, "Camera");

                Nexus::Graphics::UniformBufferView transformUniformBufferView = {};
                transformUniformBufferView.BufferHandle = m_TransformUniformBuffer;
                transformUniformBufferView.Offset = 0;
                transformUniformBufferView.Size =
                    m_TransformUniformBuffer->GetDescription().SizeInBytes;
                resourceSet->WriteUniformBuffer(
                    transformUniformBufferView, "Transform"
                );

                if (mat.DiffuseTexture.IsValid())
                {
                    Nexus::Graphics::CombinedImageSampler ciSampler = {};
                    ciSampler.ImageTexture = mat.DiffuseTexture;
                    ciSampler.ImageSampler = m_Sampler;
                    resourceSet->WriteCombinedImageSampler(
                        ciSampler, "u_DiffuseMap"
                    );
                }

                resourceSet->Flush();

                Nexus::Graphics::ResourceSetBindingDescription resourceBindingDesc =
                    {};
                resourceBindingDesc.TargetResourceSet = resourceSet;
                resourceBindingDesc.DynamicOffsets = {};
                m_CommandList->SetResourceSet(resourceBindingDesc);

                Nexus::Graphics::VertexBufferView vertexBufferView = {};
                vertexBufferView.BufferHandle = mesh->GetVertexBuffer();
                vertexBufferView.Offset = 0;
                vertexBufferView.Size = mesh->GetVertexBuffer()->GetSizeInBytes();
                m_CommandList->SetVertexBuffer(vertexBufferView, 0);

                Nexus::Graphics::IndexBufferView indexBufferView = {};
                indexBufferView.BufferHandle = mesh->GetIndexBuffer();
                indexBufferView.Offset = 0;
                indexBufferView.Size = mesh->GetIndexBuffer()->GetSizeInBytes();
                indexBufferView.BufferFormat = Nexus::Graphics::IndexFormat::UInt32;
                m_CommandList->SetIndexBuffer(indexBufferView);

                auto indexCount = mesh->GetIndexBuffer()->GetCount();

                Nexus::Graphics::DrawIndexedDescription drawDesc = {};
                drawDesc.VertexStart = 0;
                drawDesc.IndexStart = 0;
                drawDesc.InstanceStart = 0;
                drawDesc.IndexCount = indexCount;
                drawDesc.InstanceCount = 1;
                m_CommandList->DrawIndexed(drawDesc);
            }

            m_CommandList->End();

            m_CommandQueue->SubmitCommandLists(&m_CommandList, 1, nullptr);
            m_GraphicsDevice->WaitForIdle();
        }

        virtual void Update(Nexus::TimeSpan time) override
        {
            auto [width, height] = m_Window->GetWindowSize();
            m_Camera.Update(width, height, time);
        }

        virtual std::string GetInfo() const override
        {
            return "Loading and rendering a 3D model.";
        }

        virtual void OnEvent(const Nexus::Event &event) override
        {
            Nexus::EventDispatcher dispatcher = {};

            dispatcher.Subscribe<Nexus::MouseButtonPressedEventArgs>(
                [this](const Nexus::MouseButtonPressedEventArgs &args) {
                    if (args.Button == Nexus::MouseButton::Right)
                    {
                        m_CameraActive = true;
                        Nexus::GetApplication()
                            ->GetPrimaryWindow()
                            ->SetRelativeMouseMode(true);
                    }
                }
            );

            dispatcher.Subscribe<Nexus::KeyPressedEventArgs>(
                [this](const Nexus::KeyPressedEventArgs &args) {
                    if (args.ScanCode == Nexus::ScanCode::Escape)
                    {
                        m_CameraActive = false;
                        Nexus::GetApplication()
                            ->GetPrimaryWindow()
                            ->SetRelativeMouseMode(false);
                    }
                }
            );

            dispatcher.Subscribe<Nexus::MouseMovedEventArgs>(
                [this](const Nexus::MouseMovedEventArgs &args) {
                    if (m_CameraActive)
                    {
                        m_Camera.Rotate(args.Movement.first, args.Movement.second);
                    }
                }
            );

            dispatcher.Dispatch(event);
        }

      private:
        void CreatePipeline()
        {
            Nexus::Graphics::GraphicsPipelineDescription pipelineDescription;
            pipelineDescription.RasterizerStateDesc.TriangleCullMode =
                Nexus::Graphics::CullMode::CullNone;
            pipelineDescription.RasterizerStateDesc.TriangleFrontFace =
                Nexus::Graphics::FrontFace::Clockwise;
            pipelineDescription.DepthStencilDesc.EnableDepthTest = true;
            pipelineDescription.DepthStencilDesc.EnableDepthWrite = true;
            pipelineDescription.DepthStencilDesc.DepthComparisonFunction =
                Nexus::Graphics::ComparisonFunction::Less;

            pipelineDescription.VertexModule =
                Nexus::Utils::GetOrCreateCachedShaderFromSpirvFile(
                    m_GraphicsDevice,
                    "resources/demo/shaders/models/models.vert.glsl",
                    Nexus::GetApplication()->GetApplicationPath(),
                    Nexus::Graphics::ShaderStage::Vertex
                );

            pipelineDescription.FragmentModule =
                Nexus::Utils::GetOrCreateCachedShaderFromSpirvFile(
                    m_GraphicsDevice,
                    "resources/demo/shaders/models/models.frag.glsl",
                    Nexus::GetApplication()->GetApplicationPath(),
                    Nexus::Graphics::ShaderStage::Fragment
                );

            pipelineDescription.Layouts = {
                Nexus::Graphics::VertexPositionTexCoordNormalColourTangentBitangent::
                    GetLayout()
            };

            pipelineDescription.ColourTargetCount = 1;
            pipelineDescription.ColourFormats[0] =
                Nexus::GetApplication()->GetPrimarySwapchain()->GetColourFormat();
            pipelineDescription.Samples = Nexus::GetApplication()
                                              ->GetPrimarySwapchain()
                                              ->GetDescription()
                                              .Samples;

            pipelineDescription.ResourceDescription.Descriptors = {
                Nexus::Graphics::ResourceDescriptor{
                    .Name = "u_DiffuseMap",
                    .Type = Nexus::Graphics::ResourceDescriptorType::
                        CombinedImageSampler,
                    .CountOrSizeInBytes = 1
                },
                Nexus::Graphics::ResourceDescriptor{
                    .Name = "Camera",
                    .Type = Nexus::Graphics::ResourceDescriptorType::UniformBuffer,
                    .CountOrSizeInBytes = 1
                },
                Nexus::Graphics::ResourceDescriptor{
                    .Name = "Transform",
                    .Type = Nexus::Graphics::ResourceDescriptorType::UniformBuffer,
                    .CountOrSizeInBytes = 1
                }
            };

            m_Pipeline =
                m_GraphicsDevice->CreateGraphicsPipeline(pipelineDescription);

            for (size_t i = 0; i < m_Model->GetMeshes().size(); i++)
            {
                Nexus::Graphics::ResourceSetHandle resourceSet =
                    m_GraphicsDevice->CreateResourceSet(m_Pipeline);
                m_ResourceSets.push_back(resourceSet);
            }
        }

      private:
        Nexus::Graphics::CommandListHandle m_CommandList = {};
        Nexus::Graphics::PipelineHandle m_Pipeline = {};
        Nexus::Ref<Nexus::Graphics::Model> m_Model = nullptr;
        glm::vec3 m_ClearColour = {
            100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f
        };

        std::vector<Nexus::Graphics::ResourceSetHandle> m_ResourceSets = {};

        VB_UNIFORM_CAMERA_DEMO_MODELS m_CameraUniforms = {};
        Nexus::Graphics::DeviceBufferHandle m_CameraUniformBuffer = {};

        VB_UNIFORM_TRANSFORM_DEMO_MODELS m_TransformUniforms = {};
        Nexus::Graphics::DeviceBufferHandle m_TransformUniformBuffer = {};

        Nexus::Graphics::SamplerHandle m_Sampler = {};

        Nexus::FirstPersonCamera m_Camera = {};
        bool m_CameraActive = false;

        float m_Rotation = 0.0f;
    };
} // namespace Demos