#if defined(NX_PLATFORM_VULKAN)

#include "CommandListVk.hpp"
#include "BufferVk.hpp"
#include "FramebufferVk.hpp"
#include "PipelineVk.hpp"
#include "ResourceSetVk.hpp"
#include "TimingQueryVk.hpp"

namespace Nexus::Graphics
{
CommandListVk::CommandListVk(GraphicsDeviceVk *graphicsDevice, const CommandListSpecification &spec) : CommandList(spec), m_Device(graphicsDevice)
{
    for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
    {
        // create command pools
        {
            VkCommandPoolCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
            createInfo.queueFamilyIndex = m_Device->GetGraphicsFamily();
            if (vkCreateCommandPool(m_Device->GetVkDevice(), &createInfo, nullptr, &m_CommandPools[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create command pool");
            }
        }

        // create command buffers
        {
            VkCommandBufferAllocateInfo allocateInfo = {};
            allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocateInfo.commandPool = m_CommandPools[i];
            allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocateInfo.commandBufferCount = m_Device->GetSwapchainImageCount();

            if (vkAllocateCommandBuffers(m_Device->GetVkDevice(), &allocateInfo, &m_CommandBuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to allocate command buffer");
            }
        }
    }

    // create fences and semaphores
    {
        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VkSemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreCreateInfo.flags = 0;

        for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            if (vkCreateFence(m_Device->GetVkDevice(), &fenceCreateInfo, nullptr, &m_RenderFences[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create fence");
            }

            if (vkCreateSemaphore(m_Device->GetVkDevice(), &semaphoreCreateInfo, nullptr, &m_RenderSemaphores[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create semaphore");
            }
        }
    }
}

CommandListVk::~CommandListVk()
{
    for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
    {
        vkFreeCommandBuffers(m_Device->GetVkDevice(), m_CommandPools[i], 1, &m_CommandBuffers[i]);
        vkDestroyCommandPool(m_Device->GetVkDevice(), m_CommandPools[i], nullptr);
        vkDestroyFence(m_Device->GetVkDevice(), m_RenderFences[i], nullptr);
        vkDestroySemaphore(m_Device->GetVkDevice(), m_RenderSemaphores[i], nullptr);
    }
}

VkCommandBuffer &CommandListVk::GetCurrentCommandBuffer()
{
    return m_CommandBuffers[m_Device->GetCurrentFrameIndex()];
}

VkSemaphore &CommandListVk::GetCurrentSemaphore()
{
    return m_RenderSemaphores[m_Device->GetCurrentFrameIndex()];
}

VkFence &CommandListVk::GetCurrentFence()
{
    return m_RenderFences[m_Device->GetCurrentFrameIndex()];
}
} // namespace Nexus::Graphics

#endif