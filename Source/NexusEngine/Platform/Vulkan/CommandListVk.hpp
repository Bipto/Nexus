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

        virtual void SetVertexBuffer(VertexBuffer *vertexBuffer) override;
        virtual void SetIndexBuffer(IndexBuffer *indexBuffer) override;
        virtual void SetPipeline(Pipeline *pipeline) override;

        virtual void DrawElements(uint32_t start, uint32_t count) override;
        virtual void DrawIndexed(uint32_t count, uint32_t indexStart, uint32_t vertexStart) override;

        virtual void SetResourceSet(ResourceSet *resources) override;

        virtual void ClearColorTarget(uint32_t index, const ClearColorValue &color) override;
        virtual void ClearDepthTarget(const ClearDepthStencilValue &value) override;
        virtual void SetRenderTarget(RenderTarget target) override;

        virtual void SetViewport(const Viewport &viewport) override;
        virtual void SetScissor(const Scissor &scissor) override;

        const VkCommandBuffer &GetCurrentCommandBuffer();

    private:
        VkCommandPool m_CommandPools[FRAMES_IN_FLIGHT];
        VkCommandBuffer m_CommandBuffers[FRAMES_IN_FLIGHT];
        VkCommandBuffer m_CurrentCommandBuffer;
        GraphicsDeviceVk *m_Device;

        Pipeline *m_CurrentlyBoundPipeline = nullptr;
        bool m_RenderPassStarted = false;
        VkExtent2D m_RenderSize = {0, 0};

        uint32_t m_DepthAttachmentIndex = 0;
    };
}

#endif