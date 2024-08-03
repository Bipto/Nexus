#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "Nexus-Core/Graphics/CommandList.hpp"
#include "BufferOpenGL.hpp"
#include "PipelineOpenGL.hpp"

namespace Nexus::Graphics
{
    class GraphicsDeviceOpenGL;

    class CommandExecutorOpenGL
    {
    public:
        CommandExecutorOpenGL() = default;
        ~CommandExecutorOpenGL();
        void ExecuteCommands(const std::vector<RenderCommandData> &commands, GraphicsDeviceOpenGL *device);
        void Reset();

    private:
        void ExecuteCommand(SetVertexBufferCommand command, GraphicsDeviceOpenGL *device);
        void ExecuteCommand(Ref<IndexBuffer> command, GraphicsDeviceOpenGL *device);
        void ExecuteCommand(Ref<Pipeline> command, GraphicsDeviceOpenGL *device);
        void ExecuteCommand(DrawElementCommand command, GraphicsDeviceOpenGL *device);
        void ExecuteCommand(DrawIndexedCommand command, GraphicsDeviceOpenGL *device);
        void ExecuteCommand(DrawInstancedCommand command, GraphicsDeviceOpenGL *device);
        void ExecuteCommand(DrawInstancedIndexedCommand command, GraphicsDeviceOpenGL *device);
        void ExecuteCommand(UpdateResourcesCommand command, GraphicsDeviceOpenGL *device);
        void ExecuteCommand(ClearColorTargetCommand command, GraphicsDeviceOpenGL *device);
        void ExecuteCommand(ClearDepthStencilTargetCommand command, GraphicsDeviceOpenGL *device);
        void ExecuteCommand(SetRenderTargetCommand command, GraphicsDeviceOpenGL *device);
        void ExecuteCommand(SetViewportCommand command, GraphicsDeviceOpenGL *device);
        void ExecuteCommand(SetScissorCommand command, GraphicsDeviceOpenGL *device);
        void ExecuteCommand(ResolveSamplesToSwapchainCommand command, GraphicsDeviceOpenGL *device);
        void ExecuteCommand(StartTimingQueryCommand command, GraphicsDeviceOpenGL *device);
        void ExecuteCommand(StopTimingQueryCommand command, GraphicsDeviceOpenGL *device);

    private:
        WeakRef<PipelineOpenGL> m_CurrentlyBoundPipeline = {};
        RenderTarget m_CurrentRenderTarget = {};
        std::map<uint32_t, Nexus::WeakRef<Nexus::Graphics::VertexBufferOpenGL>> m_CurrentlyBoundVertexBuffers = {};
        GLenum m_IndexBufferFormat;
    };
}

#endif