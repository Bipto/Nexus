#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "Nexus/Graphics/CommandList.hpp"
#include "GraphicsDeviceVk.hpp"
#include "PipelineVk.hpp"

namespace Nexus::Graphics
{
    class CommandListVk : public CommandList
    {
    public:
        CommandListVk(GraphicsDeviceVk *graphicsDevice);
        virtual ~CommandListVk();

        virtual void Begin() override;
        virtual void End() override;

        virtual void SetVertexBuffer(Ref<VertexBuffer> vertexBuffer, uint32_t slot) override;
        virtual void SetIndexBuffer(Ref<IndexBuffer> indexBuffer) override;
        virtual void SetPipeline(Ref<Pipeline> pipeline) override;

        virtual void Draw(uint32_t start, uint32_t count) override;
        virtual void DrawIndexed(uint32_t count, uint32_t indexStart, uint32_t vertexStart) override;
        virtual void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t instanceStart) override;
        virtual void DrawInstancedIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t indexStart, uint32_t instanceStart) override;

        virtual void SetResourceSet(Ref<ResourceSet> resources) override;

        virtual void ClearColorTarget(uint32_t index, const ClearColorValue &color) override;
        virtual void ClearDepthTarget(const ClearDepthStencilValue &value) override;
        virtual void SetRenderTarget(RenderTarget target) override;

        virtual void SetViewport(const Viewport &viewport) override;
        virtual void SetScissor(const Scissor &scissor) override;
        virtual void ResolveFramebuffer(Ref<Framebuffer> source, uint32_t sourceIndex, Swapchain *target) override;

        const VkCommandBuffer &GetCurrentCommandBuffer();
        const VkFence &GetCurrentFence();
        const VkSemaphore &GetCurrentSemaphore();

    private:
        void TransitionImageLayout(VkImage image, uint32_t level, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlagBits aspectMask);

    private:
        VkCommandPool m_CommandPools[FRAMES_IN_FLIGHT];
        VkCommandBuffer m_CommandBuffers[FRAMES_IN_FLIGHT];
        VkCommandBuffer m_CurrentCommandBuffer;
        GraphicsDeviceVk *m_Device;

        Ref<Pipeline> m_CurrentlyBoundPipeline = nullptr;
        bool m_RenderPassStarted = false;
        VkExtent2D m_RenderSize = {0, 0};

        uint32_t m_DepthAttachmentIndex = 0;
        RenderTarget m_CurrentRenderTarget;

        VkSemaphore m_RenderSemaphores[FRAMES_IN_FLIGHT];
        VkFence m_RenderFences[FRAMES_IN_FLIGHT];
    };
}

#endif