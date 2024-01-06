#pragma once

#include "Nexus/Graphics/CommandList.hpp"

#include "D3D11Include.hpp"

namespace Nexus::Graphics
{
    class GraphicsDeviceD3D11;

    class CommandListD3D11 : public CommandList
    {
    public:
        CommandListD3D11(GraphicsDeviceD3D11 *graphicsDevice);

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

        const std::vector<RenderCommand> &GetRenderCommands();
        RenderCommandData &GetCurrentCommandData();
        GraphicsDeviceD3D11 *GetGraphicsDevice();

        Pipeline *GetCurrentPipeline();
        void BindPipeline(Pipeline *pipeline);

    private:
        std::vector<RenderCommand> m_Commands;
        std::vector<RenderCommandData> m_CommandData;
        uint32_t m_CommandIndex = 0;
        Pipeline *m_CurrentPipeline;

        GraphicsDeviceD3D11 *m_GraphicsDevice = nullptr;
    };
}