#pragma once

#include "Demo.hpp"

namespace Demos
{
    class HelloTriangleMeshShadersIndirect : public Demo
    {
      public:
        HelloTriangleMeshShadersIndirect(
            const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer,
            Nexus::Graphics::CommandQueueHandle commandQueue
        )
            : Demo(name, app, imGuiRenderer, commandQueue)
        {
        }

        virtual ~HelloTriangleMeshShadersIndirect()
        {
        }

        virtual void Load() override
        {
            m_CommandList = m_CommandQueue->CreateCommandList();
            CreatePipeline();

            Nexus::Graphics::IndirectMeshArguments args = {};
            args.GroupCountX = 1;
            args.GroupCountY = 1;
            args.GroupCountZ = 1;

            Nexus::Graphics::DeviceBufferDescription indirectBufferDesc = {};
            indirectBufferDesc.Access = Nexus::Graphics::BufferMemoryAccess::Upload;
            indirectBufferDesc.Usage = Nexus::Graphics::BufferUsage_Indirect;
            indirectBufferDesc.StrideInBytes = sizeof(Nexus::Graphics::IndirectDrawArguments);
            indirectBufferDesc.SizeInBytes = sizeof(Nexus::Graphics::IndirectDrawArguments);
            m_IndirectBuffer = m_GraphicsDevice->CreateDeviceBuffer(indirectBufferDesc);
            m_IndirectBuffer->SetData(&args, 0, sizeof(args));
        }

        virtual void Render(Nexus::TimeSpan time) override
        {
            auto [width, height] = m_Window->GetWindowSize();

            m_CommandList->Begin();
            Nexus::Graphics::ScopedDebugGroup debugGroup("Rendering Triangle", m_CommandList);

            m_CommandList->SetPipeline(m_Pipeline);
            Nexus::Graphics::SwapchainHandle swapchain = Nexus::GetApplication()->GetPrimarySwapchain();
            Nexus::Graphics::FramebufferHandle framebuffer = swapchain->GetCurrentFramebuffer();
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

            m_CommandList->ClearColourTarget(0, {m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, 1.0f});

            Nexus::Graphics::DrawMeshIndirectDescription drawDesc = {};
            drawDesc.IndirectBuffer = m_IndirectBuffer;
            drawDesc.Offset = 0;
            drawDesc.DrawCount = 1;
            drawDesc.Stride = m_IndirectBuffer->GetStrideInBytes();
            m_CommandList->DrawMeshIndirect(drawDesc);

            m_CommandList->End();

            m_CommandQueue->SubmitCommandLists(&m_CommandList, 1, nullptr);
            m_GraphicsDevice->WaitForIdle();
        }

        virtual void OnResize(Nexus::Point2D<uint32_t> size) override
        {
        }

        virtual void RenderUI() override
        {
        }

        virtual std::string GetInfo() const override
        {
            return "Rendering a triangle using indirect mesh shaders";
        }

      private:
        void CreatePipeline()
        {
            Nexus::Graphics::MeshletPipelineDescription pipelineDescription;
            pipelineDescription.RasterizerStateDesc.TriangleCullMode = Nexus::Graphics::CullMode::CullNone;
            pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::CounterClockwise;

            pipelineDescription.ColourTargetCount = 1;
            pipelineDescription.ColourFormats[0] = Nexus::GetApplication()->GetPrimarySwapchain()->GetColourFormat();
            pipelineDescription.Samples = Nexus::GetApplication()->GetPrimarySwapchain()->GetDescription().Samples;

            pipelineDescription.MeshModule = Nexus::Utils::GetOrCreateCachedShaderFromSpirvFile(
                m_GraphicsDevice, "resources/demo/shaders/mesh_shaders/hello_triangle_mesh.mesh.glsl",
                Nexus::GetApplication()->GetApplicationPath(), Nexus::Graphics::ShaderStage::Mesh
            );
            pipelineDescription.FragmentModule = Nexus::Utils::GetOrCreateCachedShaderFromSpirvFile(
                m_GraphicsDevice, "resources/demo/shaders/mesh_shaders/hello_triangle_mesh.frag.glsl",
                Nexus::GetApplication()->GetApplicationPath(), Nexus::Graphics::ShaderStage::Fragment
            );

            m_Pipeline = m_GraphicsDevice->CreateMeshletPipeline(pipelineDescription);
        }

      private:
        Nexus::Graphics::CommandListHandle m_CommandList = {};
        Nexus::Graphics::PipelineHandle m_Pipeline = {};
        Nexus::Graphics::DeviceBufferHandle m_IndirectBuffer = {};
        glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};
    };
} // namespace Demos