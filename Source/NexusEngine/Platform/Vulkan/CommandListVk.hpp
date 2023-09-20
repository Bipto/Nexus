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

        virtual void BeginRenderPass(RenderPass *renderPass, const RenderPassBeginInfo &beginInfo) override;
        virtual void EndRenderPass() override;

        virtual void DrawElements(uint32_t start, uint32_t count) override;
        virtual void DrawIndexed(uint32_t count, uint32_t offset) override;

        virtual void UpdateUniformBuffer(UniformBuffer *buffer, void *data, uint32_t size, uint32_t offset) override;
        virtual void SetResourceSet(ResourceSet *resources) override;

        const VkCommandBuffer &GetCurrentCommandBuffer();

    private:
        VkCommandPool m_CommandPools[FRAMES_IN_FLIGHT];
        VkCommandBuffer m_CommandBuffers[FRAMES_IN_FLIGHT];
        VkCommandBuffer m_CurrentCommandBuffer;
        GraphicsDeviceVk *m_Device;

        Pipeline *m_CurrentlyBoundPipeline = nullptr;
    };
}

#endif