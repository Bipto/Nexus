#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "CommandQueueVk.hpp"
	#include "GraphicsDeviceVk.hpp"
	#include "Nexus-Core/Graphics/CommandList.hpp"
	#include "PipelineVk.hpp"

namespace Nexus::Graphics
{
	class CommandListVk : public CommandList
	{
	  public:
		CommandListVk(GraphicsDeviceVk *graphicsDevice, CommandQueueVk *commandQueue, const CommandListDescription &spec);
		virtual ~CommandListVk();

		VkCommandBuffer &GetCurrentCommandBuffer();

	  private:
		GraphicsDeviceVk *m_Device		= nullptr;
		CommandQueueVk	 *m_Queue		= nullptr;
		VkCommandPool	  m_CommandPool = VK_NULL_HANDLE;
		VkCommandBuffer	  m_CommandBuffers[FRAMES_IN_FLIGHT];
		VkSemaphore		  m_RenderSemaphores[FRAMES_IN_FLIGHT];
	};
}	 // namespace Nexus::Graphics

#endif