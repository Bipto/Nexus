#pragma once

#if defined(NX_PLATFORM_D3D12)

#include "Nexus/Graphics/CommandList.hpp"
#include "GraphicsDeviceD3D12.hpp"

namespace Nexus::Graphics
{
    class CommandListD3D12 : public CommandList
    {
    public:
        CommandListD3D12(GraphicsDeviceD3D12 *device);
        virtual ~CommandListD3D12();

        virtual void Begin() override;
        virtual void End() override;

        virtual void BeginRenderPass(RenderPass *renderPass, const RenderPassBeginInfo &beginInfo) override;
        virtual void EndRenderPass() override;

        virtual void SetVertexBuffer(VertexBuffer *vertexBuffer) override;
        virtual void SetIndexBuffer(IndexBuffer *indexBuffer) override;
        virtual void SetPipeline(Pipeline *pipeline) override;

        virtual void DrawElements(uint32_t start, uint32_t count) override;
        virtual void DrawIndexed(uint32_t count, uint32_t offset) override;

        virtual void SetResourceSet(ResourceSet *resources) override;

        virtual void ClearColorTarget(uint32_t index, const ClearColorValue &color) override;
        virtual void ClearDepthTarget(const ClearDepthStencilValue &value) override;
        virtual void SetRenderTarget(RenderTarget target) override;

        ID3D12GraphicsCommandList7 *GetCommandList();

    private:
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator = nullptr;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> m_CommandList = nullptr;

        RenderPass *m_CurrentRenderPass = nullptr;
    };
}
#endif