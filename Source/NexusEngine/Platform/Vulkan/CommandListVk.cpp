#if defined(NX_PLATFORM_VULKAN)

#include "CommandListVk.hpp"
#include "RenderPassVk.hpp"
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
        auto vulkanPipeline = (PipelineVk *)pipeline;
        vkCmdBindPipeline(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetPipeline());

        m_CurrentlyBoundPipeline = pipeline;

        /* auto resourceSet = vulkanPipeline->GetResourceSet();
        auto uniformBufferDescriptor = resourceSet->GetUniformBufferrDescriptorSet();
        auto samplerDescriptor = resourceSet->GetSamplerDescriptorSet();
        vkCmdBindDescriptorSets(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetPipelineLayout(), 0, 1, &uniformBufferDescriptor, 0, nullptr);
        vkCmdBindDescriptorSets(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetPipelineLayout(), 1, 1, &samplerDescriptor, 0, nullptr); */
    }

    void CommandListVk::BeginRenderPass(RenderPass *renderPass, const RenderPassBeginInfo &beginInfo)
    {
        auto vulkanRenderPass = (RenderPassVk *)renderPass;

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = vulkanRenderPass->m_RenderPass;

        if (vulkanRenderPass->GetRenderPassDataType() == Nexus::Graphics::RenderPassDataType::Swapchain)
        {
            auto vulkanSwapchain = (SwapchainVk *)renderPass->GetData<Swapchain *>();
            renderPassInfo.framebuffer = vulkanSwapchain->GetCurrentFramebuffer();
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = vulkanSwapchain->m_SwapchainSize;

            std::vector<VkClearValue> clearValues(2);
            clearValues[0].color = {
                beginInfo.ClearColorValue.Red,
                beginInfo.ClearColorValue.Green,
                beginInfo.ClearColorValue.Blue,
                beginInfo.ClearColorValue.Alpha};

            clearValues[1].depthStencil.depth = beginInfo.ClearDepthStencilValue.Depth;
            clearValues[1].depthStencil.stencil = beginInfo.ClearDepthStencilValue.Stencil;

            renderPassInfo.clearValueCount = (uint32_t)clearValues.size();
            renderPassInfo.pClearValues = clearValues.data();
            vkCmdBeginRenderPass(m_CurrentCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            std::vector<VkClearAttachment> clearAttachments(2);
            std::vector<VkClearRect> clearRects(2);

            clearAttachments[0].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            clearAttachments[0].clearValue.color = {
                beginInfo.ClearColorValue.Red,
                beginInfo.ClearColorValue.Green,
                beginInfo.ClearColorValue.Blue,
                beginInfo.ClearColorValue.Alpha};
            clearAttachments[0].colorAttachment = 0;

            clearRects[0].baseArrayLayer = 0;
            clearRects[0].layerCount = 1;
            clearRects[0].rect.offset = {0, 0};
            clearRects[0].rect.extent = {vulkanSwapchain->m_SwapchainSize};

            clearAttachments[1].aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            clearAttachments[1].clearValue.depthStencil.depth = beginInfo.ClearDepthStencilValue.Depth;
            clearAttachments[1].clearValue.depthStencil.stencil = beginInfo.ClearDepthStencilValue.Stencil;
            clearAttachments[1].colorAttachment = 1;

            clearRects[1].baseArrayLayer = 0;
            clearRects[1].layerCount = 1;
            clearRects[1].rect.offset = {0, 0};
            clearRects[1].rect.extent = {vulkanSwapchain->m_SwapchainSize};

            vkCmdClearAttachments(m_CurrentCommandBuffer, clearAttachments.size(), clearAttachments.data(), clearRects.size(), clearRects.data());
        }
        else
        {
            auto framebuffer = vulkanRenderPass->m_Framebuffer;
            renderPassInfo.framebuffer = framebuffer->GetVkFramebuffer();
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = {(uint32_t)framebuffer->GetFramebufferSpecification().Width, (uint32_t)framebuffer->GetFramebufferSpecification().Height};

            int attachmentCount = 0;
            attachmentCount += framebuffer->GetColorTextureCount();
            if (framebuffer->HasDepthTexture())
            {
                attachmentCount += 1;
            }

            std::vector<VkClearValue> clearValues;

            for (int i = 0; i < attachmentCount; i++)
            {
                VkClearValue clearValue{};

                clearValue.depthStencil.depth = beginInfo.ClearDepthStencilValue.Depth;
                clearValue.depthStencil.stencil = beginInfo.ClearDepthStencilValue.Stencil;

                // NOTE: setting the color first here seems to reset the colour to red after changing the depth/stencil values for some reason
                clearValue.color = {
                    beginInfo.ClearColorValue.Red,
                    beginInfo.ClearColorValue.Green,
                    beginInfo.ClearColorValue.Blue,
                    beginInfo.ClearColorValue.Alpha};

                clearValues.push_back(clearValue);
            }

            renderPassInfo.clearValueCount = (uint32_t)clearValues.size();
            renderPassInfo.pClearValues = clearValues.data();
            vkCmdBeginRenderPass(m_CurrentCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        }
    }

    void CommandListVk::EndRenderPass()
    {
        vkCmdEndRenderPass(m_CurrentCommandBuffer);
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
        }

        m_RenderPassStarted = true;
    }

    void CommandListVk::SetViewport(const Viewport &viewport)
    {
        VkViewport vp;
        vp.x = viewport.X;
        vp.y = viewport.Y;
        vp.width = viewport.Width;
        vp.height = viewport.Height;
        vp.minDepth = viewport.MinDepth;
        vp.maxDepth = viewport.MaxDepth;
        vkCmdSetViewport(m_CurrentCommandBuffer, 0, 1, &vp);
    }

    void CommandListVk::SetScissor(const Rectangle &scissor)
    {
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