#include "CommandListVk.hpp"

namespace Nexus::Graphics
{
    CommandListVk::CommandListVk(GraphicsDeviceVk *graphicsDevice)
        : m_Device(graphicsDevice)
    {
        for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            // create command pools
            {
                VkCommandPoolCreateInfo createInfo = {};
                createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
                createInfo.queueFamilyIndex = graphicsDevice->GetGraphicsFamily();
                if (vkCreateCommandPool(graphicsDevice->GetVkDevice(), &createInfo, nullptr, &m_CommandPools[i]) != VK_SUCCESS)
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
                allocateInfo.commandBufferCount = graphicsDevice->GetVulkanSwapchain().SwapchainImageCount;

                if (vkAllocateCommandBuffers(graphicsDevice->GetVkDevice(), &allocateInfo, &m_CommandBuffers[i]) != VK_SUCCESS)
                {
                    throw std::runtime_error("Failed to allocate command buffer");
                }
            }
        }
    }

    void CommandListVk::Begin()
    {
        // get current command buffer
        {
            m_CurrentCommandBuffer = m_CommandBuffers[m_Device->GetCurrentFrameIndex()];
        }

        // reset command buffer
        {
            vkResetCommandBuffer(m_CurrentCommandBuffer, 0);
        }

        // begin command buffer
        {
            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
            if (vkBeginCommandBuffer(m_CurrentCommandBuffer, &beginInfo) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to begin command buffer");
            }
        }
    }

    void CommandListVk::End()
    {
    }

    void CommandListVk::SetVertexBuffer(Ref<VertexBuffer> vertexBuffer)
    {
    }

    void CommandListVk::SetIndexBuffer(Ref<IndexBuffer> indexBuffer)
    {
    }

    void CommandListVk::SetPipeline(Ref<Pipeline> pipeline)
    {
    }

    void CommandListVk::BeginRenderPass(Ref<RenderPass> renderPass, const RenderPassBeginInfo &beginInfo)
    {
    }

    void CommandListVk::EndRenderPass()
    {
    }

    void CommandListVk::DrawElements(uint32_t start, uint32_t count)
    {
    }

    void CommandListVk::DrawIndexed(uint32_t count, uint32_t offset)
    {
    }

    void CommandListVk::UpdateTexture(Ref<Texture> texture, Ref<Shader> shader, const TextureBinding &binding)
    {
    }

    void CommandListVk::UpdateUniformBuffer(Ref<UniformBuffer> buffer, void *data, uint32_t size, uint32_t offset)
    {
    }
}
