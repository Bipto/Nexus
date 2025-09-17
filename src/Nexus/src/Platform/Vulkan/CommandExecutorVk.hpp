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
		void ExecuteCommands(const std::vector<RenderCommandData> &commands, GraphicsDevice *device);
		void Reset();

		void SetCommandBuffer(VkCommandBuffer commandBuffer);

	  private:
		void ExecuteCommand(SetVertexBufferCommand command, GraphicsDevice *device);
		void ExecuteCommand(SetIndexBufferCommand command, GraphicsDevice *device);
		void ExecuteCommand(WeakRef<Pipeline> command, GraphicsDevice *device);
		void ExecuteCommand(DrawDescription command, GraphicsDevice *device);
		void ExecuteCommand(DrawIndexedDescription command, GraphicsDevice *device);
		void ExecuteCommand(DrawIndirectDescription command, GraphicsDevice *device);
		void ExecuteCommand(DrawIndirectIndexedDescription command, GraphicsDevice *device);
		void ExecuteCommand(DispatchDescription command, GraphicsDevice *device);
		void ExecuteCommand(DispatchIndirectDescription command, GraphicsDevice *device);
		void ExecuteCommand(DrawMeshDescription command, GraphicsDevice *device);
		void ExecuteCommand(DrawMeshIndirectDescription command, GraphicsDevice *device);
		void ExecuteCommand(Ref<ResourceSet> command, GraphicsDevice *device);
		void ExecuteCommand(ClearColorTargetCommand command, GraphicsDevice *device);
		void ExecuteCommand(ClearDepthStencilTargetCommand command, GraphicsDevice *device);
		void ExecuteCommand(RenderTarget command, GraphicsDevice *device);
		void ExecuteCommand(const Viewport &command, GraphicsDevice *device);
		void ExecuteCommand(const Scissor &command, GraphicsDevice *device);
		void ExecuteCommand(ResolveSamplesToSwapchainCommand command, GraphicsDevice *device);
		void ExecuteCommand(StartTimingQueryCommand command, GraphicsDevice *device);
		void ExecuteCommand(StopTimingQueryCommand command, GraphicsDevice *device);
		void ExecuteCommand(const CopyBufferToBufferCommand &command, GraphicsDevice *device);
		void ExecuteCommand(const CopyBufferToTextureCommand &command, GraphicsDevice *device);
		void ExecuteCommand(const CopyTextureToBufferCommand &command, GraphicsDevice *device);
		void ExecuteCommand(const CopyTextureToTextureCommand &command, GraphicsDevice *device);
		void ExecuteCommand(BeginDebugGroupCommand command, GraphicsDevice *device);
		void ExecuteCommand(EndDebugGroupCommand command, GraphicsDevice *device);
		void ExecuteCommand(InsertDebugMarkerCommand command, GraphicsDevice *device);
		void ExecuteCommand(SetBlendFactorCommand command, GraphicsDevice *device);
		void ExecuteCommand(SetStencilReferenceCommand command, GraphicsDevice *device);
		void ExecuteCommand(BuildAccelerationStructuresCommand command, GraphicsDevice *device);
		void ExecuteCommand(AccelerationStructureCopyDescription command, GraphicsDevice *Device);
		void ExecuteCommand(AccelerationStructureDeviceBufferCopyDescription command, GraphicsDevice *device);
		void ExecuteCommand(DeviceBufferAccelerationStructureCopyDescription command, GraphicsDevice *device);
		void ExecuteCommand(const PushConstantsDesc &command, GraphicsDevice *device);

		void StartRenderingToSwapchain(Ref<Swapchain> swapchain);
		void StartRenderingToFramebuffer(Ref<Framebuffer> framebuffer);
		void StopRendering();
		void TransitionFramebufferToShaderReadonly(Ref<Framebuffer> framebuffer);
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