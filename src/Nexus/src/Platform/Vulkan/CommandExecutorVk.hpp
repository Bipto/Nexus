#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "Nexus-Core/Graphics/CommandExecutor.hpp"
	#include "SwapchainVk.hpp"
	#include "Vk.hpp"

namespace Nexus::Graphics
{
	class GraphicsDeviceVk;

	class CommandExecutorVk : public CommandExecutor
	{
	  public:
		explicit CommandExecutorVk(GraphicsDeviceVk *device);
		virtual ~CommandExecutorVk();
		void ExecuteCommands(Ref<ICommandList> commandList, IGraphicsDevice *device) final;
		void Reset() final;

		void SetCommandBuffer(VkCommandBuffer commandBuffer);

	  private:
		void ExecuteCommand(const SetVertexBufferCommand &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const SetIndexBufferCommand &command, IGraphicsDevice *device) final;
		void ExecuteCommand(WeakRef<Pipeline> command, IGraphicsDevice *device) final;
		void ExecuteCommand(const DrawDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const DrawIndexedDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const DrawIndirectDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const DrawIndirectIndexedDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const DispatchDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const DispatchIndirectDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const DrawMeshDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const DrawMeshIndirectDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(Ref<IResourceSet> command, IGraphicsDevice *device) final;
		void ExecuteCommand(const ClearColorTargetCommand &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const ClearDepthStencilTargetCommand &command, IGraphicsDevice *device) final;
		void ExecuteCommand(WeakRef<IFramebuffer> command, IGraphicsDevice *device) final;
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
		void ExecuteCommand(const AccelerationStructureDeviceBufferCopyDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const DeviceBufferAccelerationStructureCopyDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const PushConstantsDesc &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const MemoryBarrierDesc &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const TextureBarrierDesc &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const BufferBarrierDesc &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const EndRenderingCommand &command, IGraphicsDevice *device) final;

		void StartRenderingToFramebuffer(Ref<IFramebuffer> framebuffer);
		void StopRendering();
		bool ValidateIsRendering();

		void BindGraphicsPipeline();
		void TryStartRendering();

	  private:
		GraphicsDeviceVk *m_Device = nullptr;

		WeakRef<Pipeline> m_CurrentlyBoundPipeline = {};
		bool			  m_Rendering			   = false;
		VkExtent2D		  m_RenderSize			   = {0, 0};

		uint32_t		  m_DepthAttachmentIndex = 0;
		Ref<IFramebuffer> m_CurrentRenderTarget;

		VkCommandBuffer m_CommandBuffer = nullptr;

		std::vector<RenderCommandData> m_Commands;
		size_t						   m_CurrentCommandIndex = 0;
	};
}	 // namespace Nexus::Graphics

#endif