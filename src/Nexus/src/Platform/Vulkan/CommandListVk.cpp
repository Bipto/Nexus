#if defined(NX_PLATFORM_VULKAN)

	#include "CommandListVk.hpp"

	#include "DeviceBufferVk.hpp"
	#include "FramebufferVk.hpp"
	#include "PipelineVk.hpp"
	#include "ResourceSetVk.hpp"
	#include "TimingQueryVk.hpp"

namespace Nexus::Graphics
{
	CommandListVk::CommandListVk(GraphicsDeviceVk *graphicsDevice, const CommandListDescription &spec) : CommandList(spec), m_Device(graphicsDevice)
	{
		// create command pool
		{
			VkCommandPoolCreateInfo createInfo = {};
			createInfo.sType				   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			createInfo.flags				   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
			createInfo.queueFamilyIndex		   = m_Device->GetGraphicsFamily();
			if (vkCreateCommandPool(m_Device->GetVkDevice(), &createInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create command pool");
			}

			std::string debugName = spec.DebugName + "- Command Pool";
			graphicsDevice->SetObjectName(VK_OBJECT_TYPE_COMMAND_POOL, (uint64_t)m_CommandPool, debugName.c_str());
		}

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			// create command buffers
			{
				VkCommandBufferAllocateInfo allocateInfo = {};
				allocateInfo.sType						 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocateInfo.commandPool				 = m_CommandPool;
				allocateInfo.level						 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocateInfo.commandBufferCount			 = FRAMES_IN_FLIGHT;

				if (vkAllocateCommandBuffers(m_Device->GetVkDevice(), &allocateInfo, &m_CommandBuffers[i]) != VK_SUCCESS)
				{
					throw std::runtime_error("Failed to allocate command buffer");
				}

				std::string debugName = spec.DebugName + std::string("- Command Buffer(") + std::to_string(i) + std::string(")");
				graphicsDevice->SetObjectName(VK_OBJECT_TYPE_COMMAND_POOL, (uint64_t)m_CommandPool, debugName.c_str());
			}
		}
	}

	CommandListVk::~CommandListVk()
	{
		for (int i = 0; i < FRAMES_IN_FLIGHT; i++) { vkFreeCommandBuffers(m_Device->GetVkDevice(), m_CommandPool, 1, &m_CommandBuffers[i]); }
		vkDestroyCommandPool(m_Device->GetVkDevice(), m_CommandPool, nullptr);
	}

	VkCommandBuffer &CommandListVk::GetCurrentCommandBuffer()
	{
		return m_CommandBuffers[m_Device->GetCurrentFrameIndex()];
	}
}	 // namespace Nexus::Graphics

#endif