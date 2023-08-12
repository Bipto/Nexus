#pragma once

#include "Core/Graphics/CommandList.hpp"
#include "GraphicsDeviceVk.hpp"

namespace Nexus::Graphics
{
    class CommandListVk : public CommandList
    {
    public:
        CommandListVk(GraphicsDeviceVk *graphicsDevice);
        virtual void Begin() override;
        virtual void End() override;

        virtual void SetVertexBuffer(Ref<VertexBuffer> vertexBuffer) override;
        virtual void SetIndexBuffer(Ref<IndexBuffer> indexBuffer) override;
        virtual void SetPipeline(Ref<Pipeline> pipeline) override;

        virtual void BeginRenderPass(Ref<RenderPass> renderPass, const RenderPassBeginInfo &beginInfo) override;
        virtual void EndRenderPass() override;

        virtual void DrawElements(uint32_t start, uint32_t count) override;
        virtual void DrawIndexed(uint32_t count, uint32_t offset) override;

        virtual void UpdateTexture(Ref<Texture> texture, Ref<Shader> shader, const TextureBinding &binding) override;
        virtual void UpdateUniformBuffer(Ref<UniformBuffer> buffer, void *data, uint32_t size, uint32_t offset) override;

    private:
        VkCommandPool m_CommandPools[FRAMES_IN_FLIGHT];
        VkCommandBuffer m_CommandBuffers[FRAMES_IN_FLIGHT];
        VkCommandBuffer m_CurrentCommandBuffer;
        GraphicsDeviceVk *m_Device;
    };
}