#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Utils.hpp"
	#include "Nexus-Core/Graphics/CommandExecutor.hpp"
	#include "Nexus-Core/Graphics/CommandList.hpp"

namespace Nexus::Graphics
{
	// forward declarations
	class SwapchainD3D12;
	class FramebufferD3D12;
	class GraphicsDeviceD3D12;
	class GraphicsPipelineD3D12;

	class CommandExecutorD3D12 : public CommandExecutor
	{
	  public:
		CommandExecutorD3D12();
		virtual ~CommandExecutorD3D12();
		virtual void ExecuteCommands(const std::vector<RenderCommandData> &commands, GraphicsDevice *device) override;
		virtual void Reset() override;

		void SetCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> commandList);

	  private:
		virtual void ExecuteCommand(SetVertexBufferCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(SetIndexBufferCommand command, GraphicsDevice *device) override;
		virtual void ExecuteCommand(WeakRef<GraphicsPipeline> command, GraphicsDevice *device) override;
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

		void SetSwapchain(SwapchainD3D12 *swapchain, GraphicsDevice *device);
		void SetFramebuffer(Ref<FramebufferD3D12> framebuffer, GraphicsDevice *device);
		void ResetPreviousRenderTargets(GraphicsDevice *device);

	  private:
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> m_CommandList = nullptr;

		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_DescriptorHandles;
		D3D12_CPU_DESCRIPTOR_HANDLE				 m_DepthHandle;

		std::optional<RenderTarget>		  m_CurrentRenderTarget	   = {};
		std::optional<Ref<GraphicsPipelineD3D12>> m_CurrentlyBoundPipeline = {};
	};
}	 // namespace Nexus::Graphics

#endif