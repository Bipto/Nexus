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
		virtual void ExecuteCommands(Ref<CommandList> commandList, GraphicsDevice *device) override;
		virtual void Reset() override;

		void SetCommandBuffer(VkCommandBuffer commandBuffer);

		virtual void ExecuteCommand(const SetVertexBufferCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const SetIndexBufferCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const SetPipelineCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const DrawDescription &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const DrawIndexedDescription &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const DrawIndirectDescription &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const DrawIndirectIndexedDescription &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const DispatchDescription &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const DispatchIndirectDescription &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const DrawMeshDescription &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const DrawMeshIndirectDescription &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(Ref<ResourceSet> command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const ClearColourTargetCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const ClearDepthStencilTargetCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(RenderTarget command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const Viewport &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const Scissor &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const ResolveSamplesToSwapchainCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const StartTimingQueryCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const StopTimingQueryCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const CopyBufferToBufferCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const CopyBufferToTextureCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const CopyTextureToBufferCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const CopyTextureToTextureCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const BeginDebugGroupCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const EndDebugGroupCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const InsertDebugMarkerCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const SetBlendFactorCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const SetStencilReferenceCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const BuildAccelerationStructuresCommand &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const AccelerationStructureCopyDescription &command, GraphicsDevice *Device) override;
		virtual void ExecuteCommand(const AccelerationStructureDeviceBufferCopyDescription &command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(const DeviceBufferAccelerationStructureCopyDescription &command, GraphicsDevice *device) override;
		void		 ExecuteCommand(const PushConstantsDesc &command, GraphicsDevice *device) final;

	  private:
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

		size_t m_CurrentCommandIndex = 0;
	};
}	 // namespace Nexus::Graphics

#endif