#pragma once

#include "CommandExecutorVk.hpp"
#include "GraphicsDeviceVk.hpp"

#include "RHI/CommandQueue.hpp"
#include <RHI/CommandList.hpp>
#include <RHI/Fence.hpp>
#include <RHI/GraphicsDevice.hpp>
#include <RHI/Swapchain.hpp>
#include <RHI/Types.hpp>

namespace Nexus::Graphics
{
	class CommandQueueVk final : public ICommandQueue
	{
	  public:
		CommandQueueVk(GraphicsDeviceVk *device, const CommandQueueDescription &description);
		virtual ~CommandQueueVk();
		const CommandQueueDescription &GetDescription() const final;
		Ref<ISwapchain>				   CreateSwapchain(const SwapchainDescription &spec) final;
		void						   SubmitCommandList(Ref<ICommandList> commandList) final;
		void						   SubmitCommandList(Ref<ICommandList> commandList, Ref<IFence> fence) final;
		void						   SubmitCommandLists(Ref<ICommandList> *commandLists, uint32_t numCommandLists) final;
		void						   SubmitCommandLists(Ref<ICommandList> *commandLists, uint32_t numCommandLists, Ref<IFence> fence) final;
		IGraphicsDevice				  *GetGraphicsDevice() final;
		bool						   WaitForIdle() final;
		VkQueue						   GetVkQueue() const;
		Ref<ICommandList>			   CreateCommandList(const CommandListDescription &spec = {}) final;

	  private:
		GraphicsDeviceVk				  *m_Device			 = nullptr;
		CommandQueueDescription			   m_Description	 = {};
		VkQueue							   m_Queue			 = VK_NULL_HANDLE;
		std::unique_ptr<CommandExecutorVk> m_CommandExecutor = nullptr;
	};
}	 // namespace Nexus::Graphics