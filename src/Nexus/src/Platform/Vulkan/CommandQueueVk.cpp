#include "CommandQueueVk.hpp"

#include "Nexus-Core/Timings/Profiler.hpp"

#include "CommandListVk.hpp"
#include "FenceVk.hpp"
#include "Vk.hpp"

namespace Nexus::Graphics
{
	CommandQueueVk::CommandQueueVk(GraphicsDeviceVk *device, const CommandQueueDescription &description)
		: m_Device(device),
		  m_Description(description)
	{
		m_Queue = Vk::GetDeviceQueue(device, description);
		Vk::SetObjectName(device->GetVkDevice(), VK_OBJECT_TYPE_QUEUE, (uint64_t)m_Queue, description.DebugName.c_str());

		m_CommandExecutor = std::make_unique<CommandExecutorVk>(device);
	}

	CommandQueueVk::~CommandQueueVk()
	{
	}

	void CommandQueueVk::SubmitCommandLists(Ref<CommandList> *commandLists, uint32_t numCommandLists, Ref<Fence> fence)
	{
		NX_PROFILE_FUNCTION();

		VkPipelineStageFlags		 waitDestStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		std::vector<VkCommandBuffer> commandBuffers(numCommandLists);

		// record the commands into the actual vulkan command list
		for (uint32_t i = 0; i < numCommandLists; i++)
		{
			Ref<CommandListVk>									   commandList = std::dynamic_pointer_cast<CommandListVk>(commandLists[i]);
			const std::vector<Nexus::Graphics::RenderCommandData> &commands	   = commandList->GetCommandData();
			m_CommandExecutor->SetCommandBuffer(commandList->GetCurrentCommandBuffer());
			m_CommandExecutor->ExecuteCommands(commandList, m_Device);
			m_CommandExecutor->Reset();
			commandBuffers[i] = commandList->GetCurrentCommandBuffer();
		}

		VkFence vulkanFence = VK_NULL_HANDLE;

		if (fence)
		{
			Ref<FenceVk> fenceVk = std::dynamic_pointer_cast<FenceVk>(fence);
			vulkanFence			 = fenceVk->GetHandle();
		}

		NX_VALIDATE(Vk::SubmitQueue(m_Device, m_Queue, commandBuffers, waitDestStageMask, vulkanFence) == VK_SUCCESS, "Failed to submit queue");
	}

	void CommandQueueVk::Present(Ref<Swapchain> swapchain)
	{
		Ref<SwapchainVk> swapchainVk = std::dynamic_pointer_cast<SwapchainVk>(swapchain);
		swapchainVk->SwapBuffers(this);
	}

	GraphicsDevice *CommandQueueVk::GetGraphicsDevice()
	{
		return m_Device;
	}

	bool CommandQueueVk::WaitForIdle()
	{
		VkResult result = vkQueueWaitIdle(m_Queue);
		if (result == VK_SUCCESS)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	VkQueue CommandQueueVk::GetVkQueue() const
	{
		return m_Queue;
	}
}	 // namespace Nexus::Graphics
