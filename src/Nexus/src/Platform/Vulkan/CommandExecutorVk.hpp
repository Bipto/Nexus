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
		void ExecuteCommands(Ref<CommandList> commandList, GraphicsDevice *device) final;
		void Reset() final;

		void SetCommandBuffer(VkCommandBuffer commandBuffer);

	  private:
		void ExecuteCommand(const SetVertexBufferCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const SetIndexBufferCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(WeakRef<Pipeline> command, GraphicsDevice *device) final;
		void ExecuteCommand(const DrawDescription &command, GraphicsDevice *device) final;
		void ExecuteCommand(const DrawIndexedDescription &command, GraphicsDevice *device) final;
		void ExecuteCommand(const DrawIndirectDescription &command, GraphicsDevice *device) final;
		void ExecuteCommand(const DrawIndirectIndexedDescription &command, GraphicsDevice *device) final;
		void ExecuteCommand(const DispatchDescription &command, GraphicsDevice *device) final;
		void ExecuteCommand(const DispatchIndirectDescription &command, GraphicsDevice *device) final;
		void ExecuteCommand(const DrawMeshDescription &command, GraphicsDevice *device) final;
		void ExecuteCommand(const DrawMeshIndirectDescription &command, GraphicsDevice *device) final;
		void ExecuteCommand(Ref<ResourceSet> command, GraphicsDevice *device) final;
		void ExecuteCommand(const ClearColorTargetCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const ClearDepthStencilTargetCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(RenderTarget command, GraphicsDevice *device) final;
		void ExecuteCommand(const Viewport &command, GraphicsDevice *device) final;
		void ExecuteCommand(const Scissor &command, GraphicsDevice *device) final;
		void ExecuteCommand(const ResolveSamplesToSwapchainCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const StartTimingQueryCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const StopTimingQueryCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const CopyBufferToBufferCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const CopyBufferToTextureCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const CopyTextureToBufferCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const CopyTextureToTextureCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const BeginDebugGroupCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const EndDebugGroupCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const InsertDebugMarkerCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const SetBlendFactorCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const SetStencilReferenceCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const BuildAccelerationStructuresCommand &command, GraphicsDevice *device) final;
		void ExecuteCommand(const AccelerationStructureCopyDescription &command, GraphicsDevice *Device) final;
		void ExecuteCommand(const AccelerationStructureDeviceBufferCopyDescription &command, GraphicsDevice *device) final;
		void ExecuteCommand(const DeviceBufferAccelerationStructureCopyDescription &command, GraphicsDevice *device) final;
		void ExecuteCommand(const PushConstantsDesc &command, GraphicsDevice *device) final;
		void ExecuteCommand(const MemoryBarrierDesc &command, GraphicsDevice *device) final;
		void ExecuteCommand(const TextureBarrierDesc &command, GraphicsDevice *device) final;
		void ExecuteCommand(const BufferBarrierDesc &command, GraphicsDevice *device) final;

		void StartRenderingToSwapchain(Ref<Swapchain> swapchain);
		void StartRenderingToFramebuffer(Ref<Framebuffer> framebuffer);
		void StopRendering();
		bool ValidateIsRendering();

		void BindGraphicsPipeline();

	  private:
		GraphicsDeviceVk *m_Device = nullptr;

		WeakRef<Pipeline> m_CurrentlyBoundPipeline = {};
		bool			  m_Rendering			   = false;
		VkExtent2D		  m_RenderSize			   = {0, 0};

		uint32_t	 m_DepthAttachmentIndex = 0;
		RenderTarget m_CurrentRenderTarget;

		VkCommandBuffer m_CommandBuffer = nullptr;

		std::vector<RenderCommandData> m_Commands;
		size_t						   m_CurrentCommandIndex = 0;
	};
}	 // namespace Nexus::Graphics

#endif