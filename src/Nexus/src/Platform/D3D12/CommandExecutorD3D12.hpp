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
		void ExecuteCommands(const std::vector<RenderCommandData> &commands, GraphicsDevice *device);
		void Reset();

		void SetCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList);

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