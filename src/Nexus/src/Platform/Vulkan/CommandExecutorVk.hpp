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
		virtual void ExecuteCommand(DrawCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(DrawIndexedCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(DrawIndirectCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(DrawIndirectIndexedCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(DispatchCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(DispatchIndirectCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(Ref<ResourceSet> command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(ClearColorTargetCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(ClearDepthStencilTargetCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(RenderTarget command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const Viewport &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const Scissor &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(ResolveSamplesToSwapchainCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(StartTimingQueryCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(StopTimingQueryCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(SetStencilRefCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(SetDepthBoundsCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(SetBlendFactorCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const BarrierDesc &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const CopyBufferToBufferCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const CopyBufferToTextureCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const CopyTextureToBufferCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const CopyTextureToTextureCommand &command, GraphicsDevice *device) override;

		void StartRenderingToSwapchain(SwapchainVk *swapchain);
		void StartRenderingToFramebuffer(Ref<Framebuffer> framebuffer);
		void StopRendering();
		void TransitionFramebufferToShaderReadonly(Ref<Framebuffer> framebuffer);
		bool ValidateIsRendering();

	  private:
		GraphicsDeviceVk *m_Device = nullptr;

		Ref<Pipeline> m_CurrentlyBoundPipeline = nullptr;
		bool		  m_Rendering			   = false;
		VkExtent2D	  m_RenderSize			   = {0, 0};

		uint32_t	 m_DepthAttachmentIndex = 0;
		RenderTarget m_CurrentRenderTarget;

		VkCommandBuffer m_CommandBuffer = nullptr;
	};
}	 // namespace Nexus::Graphics

#endif