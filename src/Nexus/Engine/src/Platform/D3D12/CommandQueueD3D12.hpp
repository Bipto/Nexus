#pragma once

#include "CommandExecutorD3D12.hpp"
#include "GraphicsDeviceD3D12.hpp"
#include "RHI/CommandList.hpp"
#include "RHI/CommandQueue.hpp"
#include "RHI/Fence.hpp"
#include "RHI/GraphicsDevice.hpp"
#include "RHI/Swapchain.hpp"
#include "RHI/Types.hpp"

namespace Nexus::Graphics
{
	class CommandQueueD3D12 final : public ICommandQueue
	{
	  public:
		CommandQueueD3D12(GraphicsDeviceD3D12 *device, const CommandQueueDescription &description);
		virtual ~CommandQueueD3D12();
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetHandle();
		const CommandQueueDescription			  &GetDescription() const final;
		SwapchainHandle							   CreateSwapchain(const SwapchainDescription &spec) final;
		void									   SubmitCommandList(Ref<ICommandList> commandList) final;
		void									   SubmitCommandList(Ref<ICommandList> commandList, Ref<IFence> fence) final;
		void									   SubmitCommandLists(Ref<ICommandList> *commandLists, uint32_t numCommandLists) final;
		void			  SubmitCommandLists(Ref<ICommandList> *commandLists, uint32_t numCommandLists, Ref<IFence> fence) final;
		IGraphicsDevice	 *GetGraphicsDevice() final;
		bool			  WaitForIdle() final;
		Ref<ICommandList> CreateCommandList(const CommandListDescription &spec = {}) final;

	  private:
		void SignalAndWait();

	  private:
		GraphicsDeviceD3D12						  *m_Device			 = nullptr;
		CommandQueueDescription					   m_Description	 = {};
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue	 = nullptr;
		std::unique_ptr<CommandExecutorD3D12>	   m_CommandExecutor = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Fence1> m_Fence	  = nullptr;
		uint64_t							 m_FenceValue = 0;
		HANDLE								 m_FenceEvent = nullptr;

		CommandQueueResourceManager m_Resources = {};
	};
}	 // namespace Nexus::Graphics