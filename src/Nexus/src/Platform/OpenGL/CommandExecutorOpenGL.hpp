#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "Nexus-Core/Graphics/CommandExecutor.hpp"
#include "Nexus-Core/Graphics/CommandList.hpp"

#include "BufferOpenGL.hpp"
#include "PipelineOpenGL.hpp"

namespace Nexus::Graphics
{
    class CommandExecutorOpenGL : public CommandExecutor
    {
    public:
        CommandExecutorOpenGL() = default;
        virtual ~CommandExecutorOpenGL();
        virtual void ExecuteCommands(const std::vector<RenderCommandData> &commands, GraphicsDevice *device) override;
        virtual void Reset() override;

    private:
        virtual void ExecuteCommand(SetVertexBufferCommand command, GraphicsDevice *device) override;
        virtual void ExecuteCommand(WeakRef<IndexBuffer> command, GraphicsDevice *device) override;
        virtual void ExecuteCommand(WeakRef<Pipeline> command, GraphicsDevice *device) override;
        virtual void ExecuteCommand(DrawElementCommand command, GraphicsDevice *device) override;
        virtual void ExecuteCommand(DrawIndexedCommand command, GraphicsDevice *device) override;
        virtual void ExecuteCommand(DrawInstancedCommand command, GraphicsDevice *device) override;
        virtual void ExecuteCommand(DrawInstancedIndexedCommand command, GraphicsDevice *device) override;
        virtual void ExecuteCommand(Ref<ResourceSet> command, GraphicsDevice *device) override;
        virtual void ExecuteCommand(ClearColorTargetCommand command, GraphicsDevice *device) override;
        virtual void ExecuteCommand(ClearDepthStencilTargetCommand command, GraphicsDevice *device) override;
        virtual void ExecuteCommand(RenderTarget command, GraphicsDevice *device) override;
        virtual void ExecuteCommand(const Viewport &command, GraphicsDevice *device) override;
        virtual void ExecuteCommand(const Scissor &command, GraphicsDevice *device) override;
        virtual void ExecuteCommand(ResolveSamplesToSwapchainCommand command, GraphicsDevice *device) override;
        virtual void ExecuteCommand(StartTimingQueryCommand command, GraphicsDevice *device) override;
        virtual void ExecuteCommand(StopTimingQueryCommand command, GraphicsDevice *device) override;

    private:
        WeakRef<PipelineOpenGL> m_CurrentlyBoundPipeline = {};
        RenderTarget m_CurrentRenderTarget = {};
        std::map<uint32_t, Nexus::WeakRef<Nexus::Graphics::VertexBufferOpenGL>> m_CurrentlyBoundVertexBuffers = {};
        GLenum m_IndexBufferFormat;
    };
}

#endif