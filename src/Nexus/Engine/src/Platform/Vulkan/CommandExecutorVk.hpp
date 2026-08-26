#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "RHI/CommandExecutor.hpp"
#include "SwapchainVk.hpp"
#include "Vk.hpp"

#include <unordered_map>

namespace Nexus::Graphics
{
    class GraphicsDeviceVk;
    class PipelineVk;

    class CommandExecutorVk : public CommandExecutor
    {
      public:
        explicit CommandExecutorVk(GraphicsDeviceVk *device);
        virtual ~CommandExecutorVk();
        void ExecuteCommands(ICommandList *commandList, IGraphicsDevice *device) final;
        void Reset() final;

      private:
        void ExecuteCommand(const SetVertexBufferCommand &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const SetIndexBufferCommand &command, IGraphicsDevice *device) final;
        void ExecuteCommand(PipelineHandle command, IGraphicsDevice *device) final;
        void ExecuteCommand(const DrawDescription &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const DrawIndexedDescription &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const DrawIndirectDescription &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const DrawIndirectIndexedDescription &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const DispatchDescription &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const DispatchIndirectDescription &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const DrawMeshDescription &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const DrawMeshIndirectDescription &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const ResourceSetBindingDescription &desc, IGraphicsDevice *device) final;
        void ExecuteCommand(const ClearColorTargetCommand &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const ClearDepthStencilTargetCommand &command, IGraphicsDevice *device) final;
        void ExecuteCommand(FramebufferHandle command, IGraphicsDevice *device) final;
        void ExecuteCommand(const Viewport &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const Scissor &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const ResolveTextureDescription &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const StartTimingQueryCommand &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const StopTimingQueryCommand &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const CopyBufferToBufferCommand &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const CopyBufferToTextureCommand &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const CopyTextureToBufferCommand &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const CopyTextureToTextureCommand &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const BeginDebugGroupCommand &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const EndDebugGroupCommand &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const InsertDebugMarkerCommand &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const SetBlendFactorCommand &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const SetStencilReferenceCommand &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const BuildAccelerationStructuresCommand &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const AccelerationStructureCopyDescription &command, IGraphicsDevice *Device) final;
        void ExecuteCommand(const AccelerationStructureDeviceBufferCopyDescription &command,
                            IGraphicsDevice *device) final;
        void ExecuteCommand(const DeviceBufferAccelerationStructureCopyDescription &command,
                            IGraphicsDevice *device) final;
        void ExecuteCommand(const PushConstantsDesc &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const BarrierGroupDescription &command, IGraphicsDevice *device) final;
        void ExecuteCommand(const TraceRaysDescription &desc, IGraphicsDevice *device) final;
        void ExecuteCommand(const EndRenderingCommand &command, IGraphicsDevice *device) final;
        void StartRenderingToFramebuffer(FramebufferHandle framebuffer);

      public:
        void BindFramebufferImpl(FramebufferHandle framebuffer);
        void StopRendering();
        bool ValidateIsRendering();

        void BindGraphicsPipeline();
        void TryStartRendering();

      public:
        GraphicsDeviceVk *m_Device = nullptr;

        PipelineHandle m_CurrentlyBoundPipeline = {};
        const ResourceSetVk *m_CurrentlyBoundResourceSet = nullptr;
        bool m_Rendering = false;
        VkExtent2D m_RenderSize = {0, 0};

        uint32_t m_DepthAttachmentIndex = 0;
        FramebufferHandle m_CurrentRenderTarget = {};

        VkCommandBuffer m_CommandBuffer = nullptr;

        std::span<const std::unique_ptr<IGraphicsCommand>> m_Commands = {};
        std::optional<CommandType> m_NextCommandType = {};
        bool m_LastCommand = false;

        std::array<CommandFunc, static_cast<size_t>(CommandType::Count)> m_DispatchTable = {};
    };
} // namespace Nexus::Graphics

#endif