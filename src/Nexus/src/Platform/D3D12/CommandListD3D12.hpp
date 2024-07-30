#pragma once

#if defined(NX_PLATFORM_D3D12)

#include "Nexus-Core/Graphics/CommandList.hpp"
#include "GraphicsDeviceD3D12.hpp"

#include "SwapchainD3D12.hpp"
#include "FramebufferD3D12.hpp"
#include "PipelineD3D12.hpp"

namespace Nexus::Graphics
{
    class CommandListD3D12 : public CommandList
    {
    public:
        CommandListD3D12(GraphicsDeviceD3D12 *device);
        virtual ~CommandListD3D12();

        virtual void Begin() override;
        virtual void End() override;

        virtual void SetVertexBuffer(Ref<VertexBuffer> vertexBuffer, uint32_t slot) override;
        virtual void SetIndexBuffer(Ref<IndexBuffer> indexBuffer) override;
        virtual void SetPipeline(Ref<Pipeline> ipeline) override;

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

        virtual void StartTimingQuery(Ref<TimingQuery> query) override;
        virtual void StopTimingQuery(Ref<TimingQuery> query) override;

        ID3D12GraphicsCommandList7 *GetCommandList();

        void SetSwapchain(SwapchainD3D12 *swapchain);
        void SetFramebuffer(Ref<FramebufferD3D12> framebuffer);
        void ResetPreviousRenderTargets();

    private:
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator = nullptr;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> m_CommandList = nullptr;

        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_DescriptorHandles;
        D3D12_CPU_DESCRIPTOR_HANDLE m_DepthHandle;

        RenderTarget m_CurrentRenderTarget = {};
        Ref<PipelineD3D12> m_CurrentlyBoundPipeline = nullptr;
    };
}
#endif