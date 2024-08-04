#if defined(NX_PLATFORM_VULKAN)

#include "CommandListVk.hpp"
#include "PipelineVk.hpp"
#include "BufferVk.hpp"
#include "ResourceSetVk.hpp"
#include "FramebufferVk.hpp"
#include "TimingQueryVk.hpp"

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

        m_CommandRecorder.Clear();
    }

    void CommandListVk::Begin()
    {
        m_CommandRecorder.Clear();
    }

    void CommandListVk::End()
    {
    }

    void CommandListVk::SetVertexBuffer(Ref<VertexBuffer> vertexBuffer, uint32_t slot)
    {
        SetVertexBufferCommand command;
        command.VertexBufferRef = vertexBuffer;
        command.Slot = slot;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListVk::SetIndexBuffer(Ref<IndexBuffer> indexBuffer)
    {
        m_CommandRecorder.PushCommand(indexBuffer);
    }

    void CommandListVk::SetPipeline(Ref<Pipeline> pipeline)
    {
        m_CommandRecorder.PushCommand(pipeline);
    }

    void CommandListVk::Draw(uint32_t start, uint32_t count)
    {
        DrawElementCommand command;
        command.Start = start;
        command.Count = count;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListVk::DrawIndexed(uint32_t count, uint32_t indexStart, uint32_t vertexStart)
    {
        DrawIndexedCommand command;
        command.VertexStart = vertexStart;
        command.IndexStart = indexStart;
        command.Count = count;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListVk::DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t instanceStart)
    {
        DrawInstancedCommand command;
        command.VertexCount = vertexCount;
        command.InstanceCount = instanceCount;
        command.VertexStart = vertexStart;
        command.InstanceStart = instanceStart;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListVk::DrawInstancedIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t indexStart, uint32_t instanceStart)
    {
        DrawInstancedIndexedCommand command;
        command.IndexCount = indexCount;
        command.InstanceCount = instanceCount;
        command.VertexStart = vertexStart;
        command.IndexStart = indexStart;
        command.InstanceStart = instanceStart;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListVk::SetResourceSet(Ref<ResourceSet> resources)
    {
        UpdateResourcesCommand command;
        command.Resources = resources;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListVk::ClearColorTarget(uint32_t index, const ClearColorValue &color)
    {
        ClearColorTargetCommand command;
        command.Index = index;
        command.Color = color;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListVk::ClearDepthTarget(const ClearDepthStencilValue &value)
    {
        ClearDepthStencilTargetCommand command;
        command.Value = value;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListVk::SetRenderTarget(RenderTarget target)
    {
        SetRenderTargetCommand command;
        command.Target = target;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListVk::SetViewport(const Viewport &viewport)
    {
        SetViewportCommand command;
        command.Viewport = viewport;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListVk::SetScissor(const Scissor &scissor)
    {
        SetScissorCommand command;
        command.Scissor = scissor;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListVk::ResolveFramebuffer(Ref<Framebuffer> source, uint32_t sourceIndex, Swapchain *target)
    {
        ResolveSamplesToSwapchainCommand command;
        command.Source = source;
        command.SourceIndex = sourceIndex;
        command.Target = target;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListVk::StartTimingQuery(Ref<TimingQuery> query)
    {
        StartTimingQueryCommand command;
        command.Query = query;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListVk::StopTimingQuery(Ref<TimingQuery> query)
    {
        StopTimingQueryCommand command;
        command.Query = query;
        m_CommandRecorder.PushCommand(command);
    }

    const CommandRecorder &CommandListVk::GetCommandRecorder()
    {
        return m_CommandRecorder;
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
}

#endif