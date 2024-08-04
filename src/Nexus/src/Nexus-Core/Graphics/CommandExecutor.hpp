#pragma once

#include "Nexus-Core/Graphics/CommandList.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

namespace Nexus::Graphics
{
    class CommandExecutor
    {
    public:
        CommandExecutor() = default;
        virtual ~CommandExecutor() {};
        virtual void ExecuteCommands(const std::vector<RenderCommandData> &commands, GraphicsDevice *device) = 0;
        virtual void Reset() = 0;

    private:
        virtual void ExecuteCommand(SetVertexBufferCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(WeakRef<IndexBuffer> command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(WeakRef<Pipeline> command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(DrawElementCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(DrawIndexedCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(DrawInstancedCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(DrawInstancedIndexedCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(UpdateResourcesCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(ClearColorTargetCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(ClearDepthStencilTargetCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(SetRenderTargetCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(SetViewportCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(SetScissorCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(ResolveSamplesToSwapchainCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(StartTimingQueryCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(StopTimingQueryCommand command, GraphicsDevice *device) = 0;
    };
};