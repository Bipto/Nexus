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
        vkEndCommandBuffer(m_CurrentCommandBuffer);
    }

    void CommandListVk::SetVertexBuffer(Ref<VertexBuffer> vertexBuffer)
    {
        Ref<VertexBufferVk> vulkanVB = std::dynamic_pointer_cast<VertexBufferVk>(vertexBuffer);

        VkBuffer vertexBuffers[] = {vulkanVB->GetBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(m_CurrentCommandBuffer, 0, 1, vertexBuffers, offsets);
    }

    void CommandListVk::SetIndexBuffer(Ref<IndexBuffer> indexBuffer)
    {
        Ref<IndexBufferVk> vulkanIB = std::dynamic_pointer_cast<IndexBufferVk>(indexBuffer);

        VkBuffer indexBufferRaw = vulkanIB->GetBuffer();
        VkIndexType indexType = GetVulkanIndexBufferFormat(vulkanIB->GetFormat());
        vkCmdBindIndexBuffer(m_CurrentCommandBuffer, indexBufferRaw, 0, indexType);
    }

    void CommandListVk::SetPipeline(Ref<Pipeline> pipeline)
    {
        Ref<PipelineVk> vulkanPipeline = std::dynamic_pointer_cast<PipelineVk>(pipeline);
        vkCmdBindPipeline(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetPipeline());

        auto resourceSet = vulkanPipeline->GetResourceSet();
        auto uniformBufferDescriptor = resourceSet->GetUniformBufferrDescriptorSet();
        auto samplerDescriptor = resourceSet->GetSamplerDescriptorSet();
        vkCmdBindDescriptorSets(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetPipelineLayout(), 0, 1, &uniformBufferDescriptor, 0, nullptr);
        vkCmdBindDescriptorSets(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetPipelineLayout(), 1, 1, &samplerDescriptor, 0, nullptr);
    }

    void CommandListVk::BeginRenderPass(Ref<RenderPass> renderPass, const RenderPassBeginInfo &beginInfo)
    {
        Ref<RenderPassVk> vulkanRenderPass = std::dynamic_pointer_cast<RenderPassVk>(renderPass);

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = vulkanRenderPass->m_RenderPass;

        if (vulkanRenderPass->GetRenderPassDataType() == Nexus::Graphics::RenderPassDataType::Swapchain)
        {
            auto vulkanSwapchain = (SwapchainVk *)m_Device->GetSwapchain();
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
        }
        else
        {
            auto framebuffer = (FramebufferVk *)vulkanRenderPass->m_Framebuffer;
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
                VkClearValue value;
                value.color = {
                    beginInfo.ClearColorValue.Red,
                    beginInfo.ClearColorValue.Green,
                    beginInfo.ClearColorValue.Blue,
                    beginInfo.ClearColorValue.Alpha};
                value.depthStencil.depth = beginInfo.ClearDepthStencilValue.Depth;
                value.depthStencil.stencil = beginInfo.ClearDepthStencilValue.Stencil;
                clearValues.push_back(value);
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

    void CommandListVk::UpdateUniformBuffer(Ref<UniformBuffer> buffer, void *data, uint32_t size, uint32_t offset)
    {
    }

    void CommandListVk::WriteTexture(Ref<Texture> texture, Ref<Pipeline> pipeline, const TextureResourceBinding &binding)
    {
        Ref<PipelineVk> pipelineVk = std::dynamic_pointer_cast<PipelineVk>(pipeline);
        auto resourceSet = pipelineVk->GetResourceSet();
        resourceSet->UpdateTexture(texture, binding.Slot);
    }

    void CommandListVk::WriteUniformBuffer(Ref<UniformBuffer> uniformBuffer, Ref<Pipeline> pipeline, uint32_t binding)
    {
        Ref<PipelineVk> pipelineVk = std::dynamic_pointer_cast<PipelineVk>(pipeline);
        auto resourceSet = pipelineVk->GetResourceSet();
        resourceSet->UpdateUniformBuffer(uniformBuffer, binding);
    }

    const VkCommandBuffer &CommandListVk::GetCurrentCommandBuffer()
    {
        return m_CurrentCommandBuffer;
    }
}

#endif