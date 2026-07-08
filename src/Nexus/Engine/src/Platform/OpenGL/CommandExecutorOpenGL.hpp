#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "DeviceBufferOpenGL.hpp"
#include "PipelineOpenGL.hpp"
#include "RHI/CommandExecutor.hpp"
#include "RHI/CommandList.hpp"
#include "ResourceSetOpenGL.hpp"

namespace Nexus::Graphics
{
    class CommandExecutorOpenGL final : public CommandExecutor
    {
      public:
        CommandExecutorOpenGL() = default;
        virtual ~CommandExecutorOpenGL();
        void ExecuteCommands(ICommandList *commandList, IGraphicsDevice *device);
        void Reset();

        void ExecuteCommand(
            const SetVertexBufferCommand &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const SetIndexBufferCommand &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(PipelineHandle command, IGraphicsDevice *device) final;
        void ExecuteCommand(
            const DrawDescription &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const DrawIndexedDescription &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const DrawIndirectDescription &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const DrawIndirectIndexedDescription &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const DispatchDescription &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const DispatchIndirectDescription &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const DrawMeshDescription &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const DrawMeshIndirectDescription &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const ResourceSetBindingDescription &desc, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const ClearColorTargetCommand &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const ClearDepthStencilTargetCommand &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            FramebufferHandle command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(const Viewport &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const Scissor &command, IGraphicsDevice *device) final;
        void ExecuteCommand(
            const ResolveTextureDescription &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const StartTimingQueryCommand &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const StopTimingQueryCommand &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const CopyBufferToBufferCommand &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const CopyBufferToTextureCommand &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const CopyTextureToBufferCommand &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const CopyTextureToTextureCommand &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const BeginDebugGroupCommand &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const EndDebugGroupCommand &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const InsertDebugMarkerCommand &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const SetBlendFactorCommand &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const SetStencilReferenceCommand &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const BuildAccelerationStructuresCommand &command,
            IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const AccelerationStructureCopyDescription &command,
            IGraphicsDevice *Device
        ) final;
        void ExecuteCommand(
            const AccelerationStructureDeviceBufferCopyDescription &command,
            IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const DeviceBufferAccelerationStructureCopyDescription &command,
            IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const PushConstantsDesc &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const BarrierGroupDescription &command, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const TraceRaysDescription &desc, IGraphicsDevice *device
        ) final;
        void ExecuteCommand(
            const EndRenderingCommand &command, IGraphicsDevice *device
        ) final;

        void BindResourceSet(GL::IOffscreenContext *context);
        void ExecuteGraphicsCommand(
            GL::IOffscreenContext *context, GraphicsPipelineOpenGL *pipeline,
            const std::map<uint32_t, Nexus::Graphics::VertexBufferView>
                &vertexBuffers,
            std::optional<Nexus::Graphics::IndexBufferView> indexBuffer,
            uint32_t vertexOffset, uint32_t instanceOffset,
            std::function<void(
                GraphicsPipelineOpenGL *pipeline, GL::IOffscreenContext *context
            )>
                drawCall
        );

      private:
        GraphicsDeviceOpenGL *m_Device = nullptr;
        PipelineHandle m_CurrentlyBoundPipeline = {};
        FramebufferHandle m_CurrentRenderTarget = {};
        std::map<uint32_t, VertexBufferView> m_CurrentlyBoundVertexBuffers = {};
        std::optional<IndexBufferView> m_BoundIndexBuffer = {};
        std::optional<ResourceSetBindingDescription> m_BoundResourceSet = {};
    };
} // namespace Nexus::Graphics

#endif