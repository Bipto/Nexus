#if defined(NX_PLATFORM_VULKAN)

#include "CommandListVk.hpp"
#include "PipelineVk.hpp"
#include "BufferVk.hpp"
#include "ResourceSetVk.hpp"
#include "FramebufferVk.hpp"

VkIndexType GetVulkanIndexBufferFormat(Nexus::Graphics::IndexBufferFormat format)
{
    switch (format)
    {
    case Nexus::Graphics::IndexBufferFormat::UInt16:
        return VK_INDEX_TYPE_UINT16;
    case Nexus::Graphics::IndexBufferFormat::UInt32:
        return VK_INDEX_TYPE_UINT32;
    }
}

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
                allocateInfo.commandBufferCount = graphicsDevice->GetSwapchainImageCount();

                if (vkAllocateCommandBuffers(graphicsDevice->GetVkDevice(), &allocateInfo, &m_CommandBuffers[i]) != VK_SUCCESS)
                {
                    throw std::runtime_error("Failed to allocate command buffer");
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
        if (m_RenderPassStarted)
        {
            vkCmdEndRenderPass(m_CurrentCommandBuffer);
        }

        vkEndCommandBuffer(m_CurrentCommandBuffer);

        m_RenderPassStarted = false;
    }

    void CommandListVk::SetVertexBuffer(VertexBuffer *vertexBuffer)
    {
        auto vulkanVB = (VertexBufferVk *)vertexBuffer;

        VkBuffer vertexBuffers[] = {vulkanVB->GetBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(m_CurrentCommandBuffer, 0, 1, vertexBuffers, offsets);
    }

    void CommandListVk::SetIndexBuffer(IndexBuffer *indexBuffer)
    {
        auto vulkanIB = (IndexBufferVk *)indexBuffer;

        VkBuffer indexBufferRaw = vulkanIB->GetBuffer();
        VkIndexType indexType = GetVulkanIndexBufferFormat(vulkanIB->GetFormat());
        vkCmdBindIndexBuffer(m_CurrentCommandBuffer, indexBufferRaw, 0, indexType);
    }

    void CommandListVk::SetPipeline(Pipeline *pipeline)
    {
        SetRenderTarget(pipeline->GetPipelineDescription().Target);
        auto vulkanPipeline = (PipelineVk *)pipeline;
        vkCmdBindPipeline(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetPipeline());
        m_CurrentlyBoundPipeline = pipeline;
    }

    void CommandListVk::DrawElements(uint32_t start, uint32_t count)
    {
        vkCmdDraw(m_CurrentCommandBuffer, count, 1, start, 0);
    }

    void CommandListVk::DrawIndexed(uint32_t count, uint32_t offset)
    {
        vkCmdDrawIndexed(m_CurrentCommandBuffer, count, 1, 0, offset, 0);
    }

    void CommandListVk::SetResourceSet(ResourceSet *resources)
    {
        auto pipelineVk = (PipelineVk *)m_CurrentlyBoundPipeline;
        auto resourceSetVk = (ResourceSetVk *)resources;

        auto uniformBufferDescriptorIndex = resourceSetVk->GetUniformBufferDescriptorIndex();
        auto samplerDescriptorIndex = resourceSetVk->GetTextureDescriptorIndex();

        if (resourceSetVk->HasUniformBuffers())
        {
            auto uniformBufferDescriptor = resourceSetVk->GetUniformBufferrDescriptorSet();
            vkCmdBindDescriptorSets(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineVk->GetPipelineLayout(), uniformBufferDescriptorIndex, 1, &uniformBufferDescriptor, 0, nullptr);
        }

        if (resourceSetVk->HasTextures())
        {
            auto samplerDescriptor = resourceSetVk->GetSamplerDescriptorSet();
            vkCmdBindDescriptorSets(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineVk->GetPipelineLayout(), samplerDescriptorIndex, 1, &samplerDescriptor, 0, nullptr);
        }
    }

    void CommandListVk::ClearColorTarget(uint32_t index, const ClearColorValue &color)
    {
        VkClearAttachment clearAttachment{};
        clearAttachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        clearAttachment.clearValue.color = {
            color.Red,
            color.Green,
            color.Blue,
            color.Alpha};
        clearAttachment.colorAttachment = index;

        VkClearRect clearRect;
        clearRect.baseArrayLayer = 0;
        clearRect.layerCount = 1;
        clearRect.rect.offset = {0, 0};
        clearRect.rect.extent = {m_RenderSize};

        vkCmdClearAttachments(m_CurrentCommandBuffer, 1, &clearAttachment, 1, &clearRect);
    }

    void CommandListVk::ClearDepthTarget(const ClearDepthStencilValue &value)
    {
        VkClearAttachment clearAttachment{};
        clearAttachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        clearAttachment.clearValue.depthStencil.depth = value.Depth;
        clearAttachment.clearValue.depthStencil.stencil = value.Stencil;
        clearAttachment.colorAttachment = m_DepthAttachmentIndex;

        VkClearRect clearRect;
        clearRect.baseArrayLayer = 0;
        clearRect.layerCount = 1;
        clearRect.rect.offset = {0, 0};
        clearRect.rect.extent = {m_RenderSize};

        vkCmdClearAttachments(m_CurrentCommandBuffer, 1, &clearAttachment, 1, &clearRect);
    }

    void CommandListVk::SetRenderTarget(RenderTarget target)
    {
        if (m_RenderPassStarted)
        {
            vkCmdEndRenderPass(m_CurrentCommandBuffer);
        }

        if (target.GetType() == RenderTargetType::Swapchain)
        {
            auto swapchain = target.GetData<Swapchain *>();
            auto vulkanSwapchain = (SwapchainVk *)swapchain;
            auto renderPass = vulkanSwapchain->GetRenderPass();
            auto framebuffer = vulkanSwapchain->GetCurrentFramebuffer();

            m_RenderSize = vulkanSwapchain->m_SwapchainSize;

            VkRenderPassBeginInfo info;
            info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            info.pNext = nullptr;
            info.framebuffer = framebuffer;
            info.renderPass = renderPass;
            info.renderArea.offset = {0, 0};
            info.renderArea.extent = vulkanSwapchain->m_SwapchainSize;
            info.clearValueCount = 0;
            info.pClearValues = nullptr;

            vkCmdBeginRenderPass(m_CurrentCommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);

            m_DepthAttachmentIndex = 1;
        }
        else
        {
            auto framebuffer = target.GetData<Framebuffer *>();
            auto vulkanFramebuffer = (FramebufferVk *)framebuffer;
            auto renderPass = vulkanFramebuffer->GetRenderPass();

            m_RenderSize = {vulkanFramebuffer->GetFramebufferSpecification().Width, vulkanFramebuffer->GetFramebufferSpecification().Height};

            VkRenderPassBeginInfo info;
            info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            info.pNext = nullptr;
            info.framebuffer = vulkanFramebuffer->GetVkFramebuffer();
            info.renderPass = renderPass;
            info.renderArea.offset = {0, 0};
            info.renderArea.extent = m_RenderSize;
            info.clearValueCount = 0;
            info.pClearValues = nullptr;

            vkCmdBeginRenderPass(m_CurrentCommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);

            m_DepthAttachmentIndex = framebuffer->GetColorTextureCount() + 1;
        }

        m_RenderPassStarted = true;
    }

    void CommandListVk::SetViewport(const Viewport &viewport)
    {
        if (viewport.Width == 0 || viewport.Height == 0)
            return;

        VkViewport vp;
        vp.x = viewport.X;
        vp.y = viewport.Height + viewport.Y;
        vp.width = viewport.Width;
        vp.height = -viewport.Height;
        vp.minDepth = viewport.MinDepth;
        vp.maxDepth = viewport.MaxDepth;
        vkCmdSetViewport(m_CurrentCommandBuffer, 0, 1, &vp);
    }

    void CommandListVk::SetScissor(const Rectangle &scissor)
    {
        if (scissor.Width == 0 || scissor.Height == 0)
            return;

        VkRect2D rect;
        rect.offset = {scissor.X, scissor.Y};
        rect.extent = {(uint32_t)scissor.Width, (uint32_t)scissor.Height};
        vkCmdSetScissor(m_CurrentCommandBuffer, 0, 1, &rect);
    }

    const VkCommandBuffer &CommandListVk::GetCurrentCommandBuffer()
    {
        return m_CurrentCommandBuffer;
    }
}

#endif