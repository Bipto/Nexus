#if defined(NX_PLATFORM_VULKAN)

#include "CommandListVk.hpp"
#include "PipelineVk.hpp"
#include "BufferVk.hpp"
#include "ResourceSetVk.hpp"
#include "FramebufferVk.hpp"
#include "TimingQueryVk.hpp"

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
                if (vkCreateFence(graphicsDevice->GetVkDevice(), &fenceCreateInfo, nullptr, &m_RenderFences[i]) != VK_SUCCESS)
                {
                    throw std::runtime_error("Failed to create fence");
                }

                if (vkCreateSemaphore(graphicsDevice->GetVkDevice(), &semaphoreCreateInfo, nullptr, &m_RenderSemaphores[i]) != VK_SUCCESS)
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

        m_CurrentRenderTarget = {};
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

    void CommandListVk::SetVertexBuffer(Ref<VertexBuffer> vertexBuffer, uint32_t slot)
    {
        auto vulkanVB = std::dynamic_pointer_cast<VertexBufferVk>(vertexBuffer);

        VkBuffer vertexBuffers[] = {vulkanVB->GetBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(m_CurrentCommandBuffer, slot, 1, vertexBuffers, offsets);
    }

    void CommandListVk::SetIndexBuffer(Ref<IndexBuffer> indexBuffer)
    {
        auto vulkanIB = std::dynamic_pointer_cast<IndexBufferVk>(indexBuffer);

        VkBuffer indexBufferRaw = vulkanIB->GetBuffer();
        VkIndexType indexType = GetVulkanIndexBufferFormat(vulkanIB->GetFormat());
        vkCmdBindIndexBuffer(m_CurrentCommandBuffer, indexBufferRaw, 0, indexType);
    }

    void CommandListVk::SetPipeline(Ref<Pipeline> pipeline)
    {
        SetRenderTarget(pipeline->GetPipelineDescription().Target);
        auto vulkanPipeline = std::dynamic_pointer_cast<PipelineVk>(pipeline);
        vkCmdBindPipeline(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetPipeline());
        m_CurrentlyBoundPipeline = pipeline;
    }

    void CommandListVk::Draw(uint32_t start, uint32_t count)
    {
        if (!m_RenderPassStarted)
        {
            return;
        }

        vkCmdDraw(m_CurrentCommandBuffer, count, 1, start, 0);
    }

    void CommandListVk::DrawIndexed(uint32_t count, uint32_t indexStart, uint32_t vertexStart)
    {
        if (!m_RenderPassStarted)
        {
            return;
        }

        vkCmdDrawIndexed(m_CurrentCommandBuffer, count, 1, indexStart, vertexStart, 0);
    }

    void CommandListVk::DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t instanceStart)
    {
        if (!m_RenderPassStarted)
        {
            return;
        }

        vkCmdDraw(m_CurrentCommandBuffer, vertexCount, instanceCount, vertexStart, instanceStart);
    }

    void CommandListVk::DrawInstancedIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t indexStart, uint32_t instanceStart)
    {
        if (!m_RenderPassStarted)
        {
            return;
        }

        vkCmdDrawIndexed(m_CurrentCommandBuffer, indexCount, instanceCount, indexStart, vertexStart, instanceStart);
    }

    void CommandListVk::SetResourceSet(Ref<ResourceSet> resources)
    {
        auto pipelineVk = std::dynamic_pointer_cast<PipelineVk>(m_CurrentlyBoundPipeline);
        auto resourceSetVk = std::dynamic_pointer_cast<ResourceSetVk>(resources);

        const auto &descriptorSets = resourceSetVk->GetDescriptorSets()[m_Device->GetCurrentFrameIndex()];
        for (const auto &set : descriptorSets)
        {
            vkCmdBindDescriptorSets(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineVk->GetPipelineLayout(), set.first, 1, &set.second, 0, nullptr);
        }
    }

    void CommandListVk::ClearColorTarget(uint32_t index, const ClearColorValue &color)
    {
        if (!m_RenderPassStarted)
        {
            return;
        }

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

        if (clearRect.rect.extent.width == 0 || clearRect.rect.extent.height == 0)
        {
            return;
        }

        vkCmdClearAttachments(m_CurrentCommandBuffer, 1, &clearAttachment, 1, &clearRect);
    }

    void CommandListVk::ClearDepthTarget(const ClearDepthStencilValue &value)
    {
        if (!m_RenderPassStarted)
        {
            return;
        }

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

        if (clearRect.rect.extent.width == 0 || clearRect.rect.extent.height == 0)
        {
            return;
        }

        vkCmdClearAttachments(m_CurrentCommandBuffer, 1, &clearAttachment, 1, &clearRect);
    }

    void CommandListVk::SetRenderTarget(RenderTarget target)
    {
        if (m_RenderPassStarted)
        {
            vkCmdEndRenderPass(m_CurrentCommandBuffer);

            if (m_CurrentRenderTarget.GetType() == RenderTargetType::Framebuffer)
            {
                auto framebuffer = std::dynamic_pointer_cast<FramebufferVk>(m_CurrentRenderTarget.GetData<Ref<Framebuffer>>());
                for (int i = 0; i < framebuffer->GetColorTextureCount(); i++)
                {
                    auto texture = framebuffer->GetVulkanColorTexture(i);
                    if (texture->GetVulkanLayout() != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                    {
                        TransitionVulkanImageLayout(texture->GetImage(), 0, texture->GetVulkanLayout(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
                        texture->SetLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                    }
                }

                if (framebuffer->HasDepthTexture())
                {
                    auto texture = std::dynamic_pointer_cast<TextureVk>(framebuffer->GetDepthTexture());
                    if (texture->GetVulkanLayout() != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                    {
                        TransitionVulkanImageLayout(texture->GetImage(), 0, texture->GetVulkanLayout(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
                        texture->SetLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                    }
                }
            }
        }

        m_CurrentRenderTarget = target;

        if (target.GetType() == RenderTargetType::Swapchain)
        {
            auto swapchain = target.GetData<Swapchain *>();
            auto vulkanSwapchain = (SwapchainVk *)swapchain;
            auto renderPass = vulkanSwapchain->GetRenderPass();
            auto framebuffer = vulkanSwapchain->GetCurrentFramebuffer();

            if (!vulkanSwapchain->IsSwapchainValid())
            {
                m_RenderPassStarted = false;
                return;
            }

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

            // transition image layouts if required
            if (vulkanSwapchain->GetColorImageLayout() == VK_IMAGE_LAYOUT_UNDEFINED)
            {
                auto swapchainColourImage = vulkanSwapchain->GetColourImage();
                TransitionVulkanImageLayout(swapchainColourImage, 0, vulkanSwapchain->GetColorImageLayout(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_ASPECT_COLOR_BIT);
            }

            if (vulkanSwapchain->GetDepthImageLayout() == VK_IMAGE_LAYOUT_UNDEFINED)
            {
                auto swapchainDepthImage = vulkanSwapchain->GetDepthImage();
                TransitionVulkanImageLayout(swapchainDepthImage, 0, vulkanSwapchain->GetDepthImageLayout(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VkImageAspectFlagBits(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT));
            }

            vulkanSwapchain->SetColorImageLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
            vulkanSwapchain->SetDepthImageLayout(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

            vkCmdBeginRenderPass(m_CurrentCommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);

            m_DepthAttachmentIndex = 1;

            m_RenderPassStarted = true;
        }
        else
        {
            auto framebuffer = target.GetData<Ref<Framebuffer>>();
            auto vulkanFramebuffer = std::dynamic_pointer_cast<FramebufferVk>(framebuffer);
            auto renderPass = vulkanFramebuffer->GetRenderPass();

            // on first frame, put the framebuffer images into the correct layout
            {
                for (uint32_t i = 0; i < vulkanFramebuffer->GetColorTextureCount(); i++)
                {
                    auto texture = vulkanFramebuffer->GetVulkanColorTexture(i);
                    if (texture->GetVulkanLayout() != VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                    {
                        TransitionVulkanImageLayout(texture->GetImage(), 0, texture->GetVulkanLayout(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
                        texture->SetLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
                    }
                }

                if (vulkanFramebuffer->HasDepthTexture())
                {
                    auto texture = vulkanFramebuffer->GetVulkanDepthTexture();
                    if (texture->GetVulkanLayout() != VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
                    {
                        TransitionVulkanImageLayout(texture->GetImage(), 0, texture->GetVulkanLayout(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VkImageAspectFlagBits(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT));
                        texture->SetLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
                    }
                }
            }

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

            /* for (int i = 0; i < vulkanFramebuffer->GetFramebufferSpecification().ColorAttachmentSpecification.Attachments.size(); i++)
            {
                vulkanFramebuffer->SetColorImageLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, i);
            }

            vulkanFramebuffer->SetDepthImageLayout(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL); */

            m_RenderPassStarted = true;
        }
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

    void CommandListVk::SetScissor(const Scissor &scissor)
    {
        if (scissor.Width == 0 || scissor.Height == 0)
            return;

        VkRect2D rect;
        rect.offset = {(int32_t)scissor.X, (int32_t)scissor.Y};
        rect.extent = {(uint32_t)scissor.Width, (uint32_t)scissor.Height};
        vkCmdSetScissor(m_CurrentCommandBuffer, 0, 1, &rect);
    }

    void CommandListVk::ResolveFramebuffer(Ref<Framebuffer> source, uint32_t sourceIndex, Swapchain *target)
    {
        if (!m_RenderPassStarted)
        {
            return;
        }

        if (sourceIndex > source->GetColorTextureCount())
        {
            return;
        }

        if (m_RenderPassStarted)
        {
            vkCmdEndRenderPass(m_CurrentCommandBuffer);
            m_RenderPassStarted = false;
        }

        auto framebufferVk = std::dynamic_pointer_cast<FramebufferVk>(source);
        auto swapchainVk = (SwapchainVk *)target;

        VkImage framebufferImage = framebufferVk->GetVulkanColorTexture(sourceIndex)->GetImage();
        VkImage swapchainImage = swapchainVk->GetColourImage();

        VkImageSubresourceLayers src;
        src.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        src.baseArrayLayer = 0;
        src.layerCount = 1;
        src.mipLevel = 0;

        VkImageSubresourceLayers dst;
        dst.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        dst.baseArrayLayer = 0;
        dst.layerCount = 1;
        dst.mipLevel = 0;

        VkImageResolve resolve;
        resolve.dstOffset = {0, 0, 0};
        resolve.dstSubresource = dst;
        resolve.extent = {framebufferVk->GetFramebufferSpecification().Width, framebufferVk->GetFramebufferSpecification().Height, 1};
        resolve.srcOffset = {0, 0, 0};
        resolve.srcSubresource = src;

        auto framebufferLayout = framebufferVk->GetVulkanColorTexture(sourceIndex)->GetVulkanLayout();
        auto swapchainLayout = swapchainVk->GetColorImageLayout();

        TransitionVulkanImageLayout(framebufferImage, 0, framebufferLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
        TransitionVulkanImageLayout(swapchainImage, 0, swapchainLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);

        vkCmdResolveImage(
            m_CurrentCommandBuffer,
            framebufferImage,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            swapchainImage,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &resolve);

        TransitionVulkanImageLayout(framebufferImage, 0, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, framebufferLayout, VK_IMAGE_ASPECT_COLOR_BIT);
        TransitionVulkanImageLayout(swapchainImage, 0, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, swapchainLayout, VK_IMAGE_ASPECT_COLOR_BIT);

        SetRenderTarget(m_CurrentRenderTarget);
    }

    void CommandListVk::StartTimingQuery(Ref<TimingQuery> query)
    {
        Ref<TimingQueryVk> queryVk = std::dynamic_pointer_cast<TimingQueryVk>(query);
        vkCmdWriteTimestamp(m_CurrentCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, queryVk->GetQueryPool(), 0);
    }

    void CommandListVk::StopTimingQuery(Ref<TimingQuery> query)
    {
        Ref<TimingQueryVk> queryVk = std::dynamic_pointer_cast<TimingQueryVk>(query);
        vkCmdWriteTimestamp(m_CurrentCommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, queryVk->GetQueryPool(), 1);
    }

    const VkCommandBuffer &CommandListVk::GetCurrentCommandBuffer()
    {
        return m_CurrentCommandBuffer;
    }

    const VkFence &CommandListVk::GetCurrentFence()
    {
        return m_RenderFences[m_Device->GetCurrentFrameIndex()];
    }

    const VkSemaphore &CommandListVk::GetCurrentSemaphore()
    {
        return m_RenderSemaphores[m_Device->GetCurrentFrameIndex()];
    }

    void CommandListVk::TransitionVulkanImageLayout(VkImage image, uint32_t level, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlagBits aspectMask)
    {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;

        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        barrier.image = image;
        barrier.subresourceRange.aspectMask = aspectMask;
        barrier.subresourceRange.baseMipLevel = level;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = 0;

        vkCmdPipelineBarrier(
            m_CurrentCommandBuffer,
            VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
            VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
            VK_DEPENDENCY_BY_REGION_BIT,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier);
    }
}

#endif