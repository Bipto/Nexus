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
		virtual void ExecuteCommands(const std::vector<RenderCommandData> &commands, GraphicsDevice *device) override;
		virtual void Reset() override;

		void SetCommandBuffer(VkCommandBuffer commandBuffer);

	  private:
		virtual void ExecuteCommand(SetVertexBufferCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(SetIndexBufferCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(WeakRef<Pipeline> command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(DrawDescription command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(DrawIndexedDescription command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(DrawIndirectDescription command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(DrawIndirectIndexedDescription command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(DispatchDescription command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(DispatchIndirectDescription command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(DrawMeshDescription command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(DrawMeshIndirectDescription command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(Ref<ResourceSet> command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(ClearColorTargetCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(ClearDepthStencilTargetCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(RenderTarget command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const Viewport &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const Scissor &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(ResolveSamplesToSwapchainCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(StartTimingQueryCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(StopTimingQueryCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const CopyBufferToBufferCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const CopyBufferToTextureCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const CopyTextureToBufferCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const CopyTextureToTextureCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(BeginDebugGroupCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(EndDebugGroupCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(InsertDebugMarkerCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(SetBlendFactorCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(SetStencilReferenceCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(BuildAccelerationStructuresCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(AccelerationStructureCopyDescription command, GraphicsDevice *Device) override;
		virtual void ExecuteCommand(AccelerationStructureDeviceBufferCopyDescription command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(DeviceBufferAccelerationStructureCopyDescription command, GraphicsDevice *device) override;

		void StartRenderingToSwapchain(Ref<Swapchain> swapchain);
		void StartRenderingToFramebuffer(Ref<Framebuffer> framebuffer);
		void StopRendering();
		void TransitionFramebufferToShaderReadonly(Ref<Framebuffer> framebuffer);
		bool ValidateIsRendering();

		void BindGraphicsPipeline();

	  private:
		GraphicsDeviceVk *m_Device = nullptr;

		WeakRef<Pipeline> m_CurrentlyBoundPipeline = {};
		bool		  m_Rendering			   = false;
		VkExtent2D	  m_RenderSize			   = {0, 0};

		uint32_t	 m_DepthAttachmentIndex = 0;
		RenderTarget m_CurrentRenderTarget;

		VkCommandBuffer m_CommandBuffer = nullptr;

		std::vector<RenderCommandData> m_Commands;
		size_t						   m_CurrentCommandIndex = 0;
	};
}	 // namespace Nexus::Graphics

#endif