#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Utils.hpp"
	#include "Nexus-Core/Graphics/CommandExecutor.hpp"
	#include "Nexus-Core/Graphics/CommandList.hpp"

typedef void(WINAPI *PIXBeginEventFn)(ID3D12GraphicsCommandList *, UINT64, PCSTR);
typedef void(WINAPI *PIXEndEventFn)(ID3D12GraphicsCommandList *);
typedef void(WINAPI *PIXSetMarkerFn)(ID3D12GraphicsCommandList *, UINT64, PCSTR);

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
		CommandExecutorD3D12(Microsoft::WRL::ComPtr<ID3D12Device9> device);
		virtual ~CommandExecutorD3D12();
		void ExecuteCommands(Ref<CommandList> commandList, GraphicsDevice *device) final;
		void Reset() final;

		void SetCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList);

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

		void SetSwapchain(WeakRef<Swapchain> swapchain, GraphicsDevice *device);
		void SetFramebuffer(WeakRef<Framebuffer> framebuffer, GraphicsDevice *device);
		void ResetPreviousRenderTargets(GraphicsDevice *device);

		void CreateDrawIndirectSignatureCommand();
		void CreateDrawIndexedIndirectSignatureCommand();
		void CreateDispatchIndirectSignatureCommand();

		Microsoft::WRL::ComPtr<ID3D12CommandSignature> GetOrCreateIndirectCommandSignature(D3D12_INDIRECT_ARGUMENT_TYPE type, size_t stride);

	  private:
		Microsoft::WRL::ComPtr<ID3D12Device9>			   m_Device		 = nullptr;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> m_CommandList = nullptr;

		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_DescriptorHandles;
		D3D12_CPU_DESCRIPTOR_HANDLE				 m_DepthHandle;

		std::optional<RenderTarget>	 m_CurrentRenderTarget	  = {};
		std::optional<Ref<Pipeline>> m_CurrentlyBoundPipeline = {};

		Microsoft::WRL::ComPtr<ID3D12CommandSignature> m_DrawIndirectCommandSignature		 = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandSignature> m_DrawIndexedIndirectCommandSignature = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandSignature> m_DispatchIndirectCommandSignature	 = nullptr;

		std::map<D3D12_INDIRECT_ARGUMENT_TYPE, std::map<size_t, Microsoft::WRL::ComPtr<ID3D12CommandSignature>>> m_IndirectCommandSignatures = {};

		HMODULE			m_PixModule		= NULL;
		PIXBeginEventFn m_PIXBeginEvent = NULL;
		PIXEndEventFn	m_PIXEndEvent	= NULL;
		PIXSetMarkerFn	m_PIXSetMarker	= NULL;
	};
}	 // namespace Nexus::Graphics

#endif