#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Utils.hpp"
	#include "RHI/CommandExecutor.hpp"
	#include "RHI/CommandList.hpp"

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
	class ResourceSetD3D12;
	class DeviceBufferD3D12;
	class CommandQueueD3D12;

	struct D3D12ReadbackBufferCopyOperation
	{
		DeviceBufferD3D12 *ReadbackBuffer	   = nullptr;
		DeviceBufferD3D12 *TargetBuffer		   = nullptr;
		uint32_t		   Height			   = 0;
		uint32_t		   SourceRowPitch	   = 0;
		uint32_t		   DestinationRowPitch = 0;
		uint32_t		   NumRows			   = 0;
	};

	class CommandExecutorD3D12 : public CommandExecutor
	{
	  public:
		CommandExecutorD3D12(Microsoft::WRL::ComPtr<ID3D12Device9> device);
		virtual ~CommandExecutorD3D12();
		void ExecuteCommands(Ref<ICommandList> commandList, IGraphicsDevice *device) final;
		void Reset() final;

		void SetCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList);
		void SetCommandQueue(CommandQueueD3D12 *commandQueue);

		void FlushReadbacks(IGraphicsDevice *device);

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
		void ExecuteCommand(const AccelerationStructureDeviceBufferCopyDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const DeviceBufferAccelerationStructureCopyDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const PushConstantsDesc &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const BarrierGroupDescription &command, IGraphicsDevice *device) final;
		void ExecuteCommand(const TraceRaysDescription &desc, IGraphicsDevice *device) final;
		void ExecuteCommand(const EndRenderingCommand &command, IGraphicsDevice *device) final;

		void SetFramebuffer(FramebufferHandle framebuffer, IGraphicsDevice *device);
		void ResetPreviousRenderTargets(IGraphicsDevice *device);

		void CreateDrawIndirectSignatureCommand();
		void CreateDrawIndexedIndirectSignatureCommand();
		void CreateDispatchIndirectSignatureCommand();

		Microsoft::WRL::ComPtr<ID3D12CommandSignature> GetOrCreateIndirectCommandSignature(D3D12_INDIRECT_ARGUMENT_TYPE type, size_t stride);

	  private:
		void			   InsertResourceBarrier(const TextureBarrierDesc &command);
		void			   InsertTextureBarrier(const TextureBarrierDesc &command);
		Ref<IDeviceBuffer> CreateStagingBuffer(size_t size, bool upload, IGraphicsDevice *device);

	  private:
		Microsoft::WRL::ComPtr<ID3D12Device9>			   m_Device		 = nullptr;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> m_CommandList = nullptr;

		CommandQueueD3D12 *m_CommandQueue = nullptr;

		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_DescriptorHandles = {};
		D3D12_CPU_DESCRIPTOR_HANDLE				 m_DepthHandle		 = {};

		Ref<ResourceSetD3D12> m_CurrentlyBoundResourceSet = nullptr;

		FramebufferHandle			 m_CurrentFramebuffer	  = {};
		std::optional<Ref<Pipeline>> m_CurrentlyBoundPipeline = {};

		Microsoft::WRL::ComPtr<ID3D12CommandSignature> m_DrawIndirectCommandSignature		 = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandSignature> m_DrawIndexedIndirectCommandSignature = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandSignature> m_DispatchIndirectCommandSignature	 = nullptr;

		std::map<D3D12_INDIRECT_ARGUMENT_TYPE, std::map<size_t, Microsoft::WRL::ComPtr<ID3D12CommandSignature>>> m_IndirectCommandSignatures = {};

		HMODULE			m_PixModule		= NULL;
		PIXBeginEventFn m_PIXBeginEvent = NULL;
		PIXEndEventFn	m_PIXEndEvent	= NULL;
		PIXSetMarkerFn	m_PIXSetMarker	= NULL;

		std::vector<Ref<IDeviceBuffer>>				  m_UploadBuffers  = {};
		std::vector<D3D12ReadbackBufferCopyOperation> m_ReadbackCopies = {};
	};
}	 // namespace Nexus::Graphics

#endif