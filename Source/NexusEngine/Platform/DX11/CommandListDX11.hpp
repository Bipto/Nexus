#pragma once

#include "Nexus/Graphics/CommandList.hpp"

#include "DX11.hpp"

#include <array>

namespace Nexus::Graphics
{
    class GraphicsDeviceDX11;

    class CommandListDX11 : public CommandList
    {
    public:
        CommandListDX11(GraphicsDeviceDX11 *graphicsDevice);

        virtual void Begin() override;
        virtual void End() override;

        virtual void BeginRenderPass(Ref<RenderPass> renderPass, const RenderPassBeginInfo &beginInfo) override;
        virtual void EndRenderPass() override;

        virtual void SetVertexBuffer(Ref<VertexBuffer> vertexBuffer) override;
        virtual void SetIndexBuffer(Ref<IndexBuffer> indexBuffer) override;
        virtual void SetPipeline(Ref<Pipeline> pipeline) override;

        virtual void DrawElements(uint32_t start, uint32_t count) override;
        virtual void DrawIndexed(uint32_t count, uint32_t offset) override;

        virtual void UpdateUniformBuffer(Ref<UniformBuffer> buffer, void *data, uint32_t size, uint32_t offset) override;
        virtual void SetResourceSet(Ref<ResourceSet> resources) override;

        const std::vector<RenderCommand> &GetRenderCommands();
        RenderCommandData &GetCurrentCommandData();
        GraphicsDeviceDX11 *GetGraphicsDevice();

        Ref<Pipeline> GetCurrentPipeline();
        void BindPipeline(Ref<Pipeline> pipeline);

    private:
        std::vector<RenderCommand> m_Commands;
        std::vector<RenderCommandData> m_CommandData;
        uint32_t m_CommandIndex = 0;

        RenderPassBeginInfo m_CommandListBeginInfo;
        Ref<Pipeline> m_CurrentPipeline;

        GraphicsDeviceDX11 *m_GraphicsDevice = nullptr;
    };
}