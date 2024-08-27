#if defined(NX_PLATFORM_VULKAN)

#include "CommandExecutorVk.hpp"

#include "BufferVk.hpp"
#include "FramebufferVk.hpp"
#include "PipelineVk.hpp"
#include "ResourceSetVk.hpp"
#include "TimingQueryVk.hpp"

namespace Nexus::Graphics
{
CommandExecutorVk::CommandExecutorVk(GraphicsDeviceVk *device) : m_Device(device)
{
}

CommandExecutorVk::~CommandExecutorVk()
{
}

void CommandExecutorVk::ExecuteCommands(const std::vector<RenderCommandData> &commands, GraphicsDevice *device)
{
    // begin
    {
        // reset command buffer
        {
            vkResetCommandBuffer(m_CommandBuffer, 0);
        }

        // begin command buffer
        {
            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
            if (vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to begin command buffer");
            }
        }

        m_CurrentRenderTarget = {};
    }

    // execute commands
    {
        for (const auto &element : commands)
        {
            std::visit([&](auto &&arg) { ExecuteCommand(arg, device); }, element);
        }
    }

    // end
    {
        if (m_RenderPassStarted)
        {
            vkCmdEndRenderPass(m_CommandBuffer);
        }

        vkEndCommandBuffer(m_CommandBuffer);

        m_RenderPassStarted = false;
    }
}

void CommandExecutorVk::Reset()
{
}

void CommandExecutorVk::SetCommandBuffer(VkCommandBuffer commandBuffer)
{
    m_CommandBuffer = commandBuffer;
}

void CommandExecutorVk::ExecuteCommand(SetVertexBufferCommand command, GraphicsDevice *device)
{
    if (command.VertexBufferRef.expired())
    {
        NX_ERROR("Attempting to bind an invalid vertex buffer");
        return;
    }

    auto vulkanVB = std::dynamic_pointer_cast<VertexBufferVk>(command.VertexBufferRef.lock());

    VkBuffer vertexBuffers[] = {vulkanVB->GetBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(m_CommandBuffer, command.Slot, 1, vertexBuffers, offsets);
}

void CommandExecutorVk::ExecuteCommand(WeakRef<IndexBuffer> command, GraphicsDevice *device)
{
    if (command.expired())
    {
        NX_ERROR("Attempting to bind an invalid index buffer");
        return;
    }

    auto vulkanIB = std::dynamic_pointer_cast<IndexBufferVk>(command.lock());

    VkBuffer indexBufferRaw = vulkanIB->GetBuffer();
    VkIndexType indexType = Vk::GetVulkanIndexBufferFormat(vulkanIB->GetFormat());
    vkCmdBindIndexBuffer(m_CommandBuffer, indexBufferRaw, 0, indexType);
}

void CommandExecutorVk::ExecuteCommand(WeakRef<Pipeline> command, GraphicsDevice *device)
{
    if (command.expired())
    {
        NX_ERROR("Attempting to bind an invalid pipeline");
        return;
    }

    auto vulkanPipeline = std::dynamic_pointer_cast<PipelineVk>(command.lock());

    ExecuteCommand(vulkanPipeline->GetPipelineDescription().Target, device);
    vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetPipeline());
    m_CurrentlyBoundPipeline = vulkanPipeline;
}

void CommandExecutorVk::ExecuteCommand(DrawElementCommand command, GraphicsDevice *device)
{
    if (!m_RenderPassStarted)
    {
        return;
    }

    vkCmdDraw(m_CommandBuffer, command.Count, 1, command.Start, 0);
}

void CommandExecutorVk::ExecuteCommand(DrawIndexedCommand command, GraphicsDevice *device)
{
    if (!m_RenderPassStarted)
    {
        return;
    }

    vkCmdDrawIndexed(m_CommandBuffer, command.Count, 1, command.IndexStart, command.VertexStart, 0);
}

void CommandExecutorVk::ExecuteCommand(DrawInstancedCommand command, GraphicsDevice *device)
{
    if (!m_RenderPassStarted)
    {
        return;
    }

    vkCmdDraw(m_CommandBuffer, command.VertexCount, command.InstanceCount, command.VertexStart, command.InstanceStart);
}

void CommandExecutorVk::ExecuteCommand(DrawInstancedIndexedCommand command, GraphicsDevice *device)
{
    if (!m_RenderPassStarted)
    {
        return;
    }

    vkCmdDrawIndexed(m_CommandBuffer, command.IndexCount, command.InstanceCount, command.IndexStart, command.VertexStart, command.InstanceStart);
}

void CommandExecutorVk::ExecuteCommand(Ref<ResourceSet> command, GraphicsDevice *device)
{
    auto pipelineVk = std::dynamic_pointer_cast<PipelineVk>(m_CurrentlyBoundPipeline);
    auto resourceSetVk = std::dynamic_pointer_cast<ResourceSetVk>(command);

    for (const auto [name, combinedImageSampler] : resourceSetVk->GetBoundCombinedImageSamplers())
    {
        Ref<TextureVk> texture = std::dynamic_pointer_cast<TextureVk>(combinedImageSampler.ImageTexture.lock());

        for (uint32_t level = 0; level < texture->GetLevels(); level++)
        {
            m_Device->TransitionVulkanImageLayout(m_CommandBuffer, texture->GetImage(), level, texture->GetImageLayout(level), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_ASPECT_COLOR_BIT);
            texture->SetImageLayout(level, VK_IMAGE_LAYOUT_GENERAL);
        }
    }

    const auto &descriptorSets = resourceSetVk->GetDescriptorSets()[m_Device->GetCurrentFrameIndex()];
    for (const auto &set : descriptorSets)
    {
        vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineVk->GetPipelineLayout(), set.first, 1, &set.second, 0, nullptr);
    }
}

void CommandExecutorVk::ExecuteCommand(ClearColorTargetCommand command, GraphicsDevice *device)
{
    if (!m_RenderPassStarted)
    {
        return;
    }

    VkClearAttachment clearAttachment{};
    clearAttachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    clearAttachment.clearValue.color = {command.Color.Red, command.Color.Green, command.Color.Blue, command.Color.Alpha};
    clearAttachment.colorAttachment = command.Index;

    VkClearRect clearRect;
    clearRect.baseArrayLayer = 0;
    clearRect.layerCount = 1;
    clearRect.rect.offset = {0, 0};
    clearRect.rect.extent = {m_RenderSize};

    if (clearRect.rect.extent.width == 0 || clearRect.rect.extent.height == 0)
    {
        return;
    }

    vkCmdClearAttachments(m_CommandBuffer, 1, &clearAttachment, 1, &clearRect);
}

void CommandExecutorVk::ExecuteCommand(ClearDepthStencilTargetCommand command, GraphicsDevice *device)
{
    if (!m_RenderPassStarted)
    {
        return;
    }

    VkClearAttachment clearAttachment{};
    clearAttachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    clearAttachment.clearValue.depthStencil.depth = command.Value.Depth;
    clearAttachment.clearValue.depthStencil.stencil = command.Value.Stencil;
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

    vkCmdClearAttachments(m_CommandBuffer, 1, &clearAttachment, 1, &clearRect);
}

void CommandExecutorVk::ExecuteCommand(RenderTarget command, GraphicsDevice *device)
{
    if (m_RenderPassStarted)
    {
        vkCmdEndRenderPass(m_CommandBuffer);
    }

    m_CurrentRenderTarget = command;

    if (m_CurrentRenderTarget.GetType() == RenderTargetType::Swapchain)
    {
        auto swapchain = m_CurrentRenderTarget.GetData<Swapchain *>();
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
        if (vulkanSwapchain->GetColorImageLayout() != VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
        {
            auto swapchainColourImage = vulkanSwapchain->GetColourImage();

            m_Device->ImmediateSubmit([&](VkCommandBuffer cmd) {
                m_Device->TransitionVulkanImageLayout(cmd, swapchainColourImage, 0, vulkanSwapchain->GetColorImageLayout(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                                      VK_IMAGE_ASPECT_COLOR_BIT);
            });
        }

        if (vulkanSwapchain->GetDepthImageLayout() != VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            auto swapchainDepthImage = vulkanSwapchain->GetDepthImage();
            m_Device->ImmediateSubmit([&](VkCommandBuffer cmd) {
                m_Device->TransitionVulkanImageLayout(cmd, swapchainDepthImage, 0, vulkanSwapchain->GetDepthImageLayout(), VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
                                                      VkImageAspectFlagBits(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT));
            });
        }

        vulkanSwapchain->SetColorImageLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        vulkanSwapchain->SetDepthImageLayout(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

        vkCmdBeginRenderPass(m_CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);

        m_DepthAttachmentIndex = 1;

        m_RenderPassStarted = true;
    }
    else
    {
        auto framebuffer = m_CurrentRenderTarget.GetData<Ref<Framebuffer>>();
        auto vulkanFramebuffer = std::dynamic_pointer_cast<FramebufferVk>(framebuffer);
        auto renderPass = vulkanFramebuffer->GetRenderPass();

        m_RenderSize = {vulkanFramebuffer->GetFramebufferSpecification().Width, vulkanFramebuffer->GetFramebufferSpecification().Height};

        for (size_t i = 0; i < vulkanFramebuffer->GetColorTextureCount(); i++)
        {
            Ref<TextureVk> framebufferTexture = vulkanFramebuffer->GetVulkanColorTexture(i);

            for (uint32_t j = 0; j < framebufferTexture->GetLevels(); j++)
            {
                m_Device->TransitionVulkanImageLayout(m_CommandBuffer, framebufferTexture->GetImage(), j, framebufferTexture->GetImageLayout(j), VK_IMAGE_LAYOUT_GENERAL,
                                                      VK_IMAGE_ASPECT_COLOR_BIT);
            }
        }

        if (vulkanFramebuffer->HasDepthTexture())
        {
            Ref<TextureVk> depthTexture = vulkanFramebuffer->GetVulkanDepthTexture();

            for (size_t level = 0; level < depthTexture->GetLevels(); level++)
            {
                m_Device->TransitionVulkanImageLayout(m_CommandBuffer, depthTexture->GetImage(), level, depthTexture->GetImageLayout(level), VK_IMAGE_LAYOUT_GENERAL,
                                                      VkImageAspectFlagBits(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT));
            }
        }

        VkRenderPassBeginInfo info;
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.pNext = nullptr;
        info.framebuffer = vulkanFramebuffer->GetVkFramebuffer();
        info.renderPass = renderPass;
        info.renderArea.offset = {0, 0};
        info.renderArea.extent = m_RenderSize;
        info.clearValueCount = 0;
        info.pClearValues = nullptr;

        vkCmdBeginRenderPass(m_CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);

        m_DepthAttachmentIndex = framebuffer->GetColorTextureCount() + 1;
        m_RenderPassStarted = true;
    }
}

void CommandExecutorVk::ExecuteCommand(const Viewport &command, GraphicsDevice *device)
{
    if (command.Width == 0 || command.Height == 0)
        return;

    VkViewport vp;
    vp.x = command.X;
    vp.y = command.Height + command.Y;
    vp.width = command.Width;
    vp.height = -command.Height;
    vp.minDepth = command.MinDepth;
    vp.maxDepth = command.MaxDepth;
    vkCmdSetViewport(m_CommandBuffer, 0, 1, &vp);
}

void CommandExecutorVk::ExecuteCommand(const Scissor &command, GraphicsDevice *device)
{
    if (command.Width == 0 || command.Height == 0)
        return;

    VkRect2D rect;
    rect.offset = {(int32_t)command.X, (int32_t)command.Y};
    rect.extent = {(uint32_t)command.Width, (uint32_t)command.Height};
    vkCmdSetScissor(m_CommandBuffer, 0, 1, &rect);
}

void CommandExecutorVk::ExecuteCommand(ResolveSamplesToSwapchainCommand command, GraphicsDevice *device)
{
    if (!m_RenderPassStarted)
    {
        return;
    }

    if (command.SourceIndex > command.Source.lock()->GetColorTextureCount())
    {
        return;
    }

    if (m_RenderPassStarted)
    {
        vkCmdEndRenderPass(m_CommandBuffer);
        m_RenderPassStarted = false;
    }

    auto framebufferVk = std::dynamic_pointer_cast<FramebufferVk>(command.Source.lock());
    auto swapchainVk = (SwapchainVk *)command.Target;

    VkImage framebufferImage = framebufferVk->GetVulkanColorTexture(command.SourceIndex)->GetImage();
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

    auto framebufferLayout = framebufferVk->GetVulkanColorTexture(command.SourceIndex)->GetImageLayout(0);
    auto swapchainLayout = swapchainVk->GetColorImageLayout();

    vkCmdResolveImage(m_CommandBuffer, framebufferImage, framebufferLayout, swapchainImage, swapchainLayout, 1, &resolve);

    ExecuteCommand(m_CurrentRenderTarget, device);
}

void CommandExecutorVk::ExecuteCommand(StartTimingQueryCommand command, GraphicsDevice *device)
{
    Ref<TimingQueryVk> queryVk = std::dynamic_pointer_cast<TimingQueryVk>(command.Query.lock());
    vkCmdResetQueryPool(m_CommandBuffer, queryVk->GetQueryPool(), 0, 2);
    vkCmdWriteTimestamp(m_CommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, queryVk->GetQueryPool(), 0);
}

void CommandExecutorVk::ExecuteCommand(StopTimingQueryCommand command, GraphicsDevice *device)
{
    Ref<TimingQueryVk> queryVk = std::dynamic_pointer_cast<TimingQueryVk>(command.Query.lock());
    vkCmdWriteTimestamp(m_CommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, queryVk->GetQueryPool(), 1);
}
} // namespace Nexus::Graphics

#endif