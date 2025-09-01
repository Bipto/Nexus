#pragma once

#include "CommandExecutorVk.hpp"
#include "Nexus-Core/Graphics/CommandQueue.hpp"

#include "GraphicsDeviceVk.hpp"
#include <Nexus-Core/Graphics/CommandList.hpp>
#include <Nexus-Core/Graphics/Fence.hpp>
#include <Nexus-Core/Graphics/GraphicsDevice.hpp>
#include <Nexus-Core/Graphics/Swapchain.hpp>
#include <Nexus-Core/Types.hpp>

namespace Nexus::Graphics
{
	class CommandQueueVk final : public ICommandQueue
	{
	  public:
		CommandQueueVk(GraphicsDeviceVk *device, const CommandQueueDescription &description);
		virtual ~CommandQueueVk();
		void			SubmitCommandLists(Ref<CommandList> *commandLists, uint32_t numCommandLists, Ref<Fence> fence) final;
		void			Present(Ref<Swapchain> swapchain) final;
		GraphicsDevice *GetGraphicsDevice() final;

	  private:
		GraphicsDeviceVk				  *m_Device			 = nullptr;
		CommandQueueDescription			   m_Description	 = {};
		VkQueue							   m_Queue			 = VK_NULL_HANDLE;
		std::unique_ptr<CommandExecutorVk> m_CommandExecutor = nullptr;
	};
}	 // namespace Nexus::Graphics