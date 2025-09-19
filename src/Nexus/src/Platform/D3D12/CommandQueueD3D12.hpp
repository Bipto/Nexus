#pragma once

#include "CommandExecutorD3D12.hpp"
#include "GraphicsDeviceD3D12.hpp"
#include "Nexus-Core/Graphics/CommandQueue.hpp"
#include <Nexus-Core/Graphics/CommandList.hpp>
#include <Nexus-Core/Graphics/Fence.hpp>
#include <Nexus-Core/Graphics/GraphicsDevice.hpp>
#include <Nexus-Core/Graphics/Swapchain.hpp>
#include <Nexus-Core/Types.hpp>

namespace Nexus::Graphics
{
	class CommandQueueD3D12 final : public ICommandQueue
	{
	  public:
		CommandQueueD3D12(GraphicsDeviceD3D12 *device, const CommandQueueDescription &description);
		virtual ~CommandQueueD3D12();
		void			SubmitCommandLists(Ref<CommandList> *commandLists, uint32_t numCommandLists, Ref<Fence> fence) final;
		void			Present(Ref<Swapchain> swapchain) final;
		GraphicsDevice *GetGraphicsDevice() final;
		bool			WaitForIdle() final;

	  private:
		GraphicsDeviceD3D12						  *m_Device			 = nullptr;
		CommandQueueDescription					   m_Description	 = {};
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue	 = nullptr;
		std::unique_ptr<CommandExecutorD3D12>	   m_CommandExecutor = nullptr;

		Ref<Fence> m_Fence = nullptr;
	};
}	 // namespace Nexus::Graphics