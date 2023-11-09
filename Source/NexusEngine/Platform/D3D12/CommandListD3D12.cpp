#include "CommandListD3D12.hpp"

#include "SwapchainD3D12.hpp"
#include "PipelineD3D12.hpp"
#include "BufferD3D12.hpp"
#include "ResourceSetD3D12.hpp"

namespace Nexus::Graphics
{
    DXGI_FORMAT GetD3D12IndexBufferFormat(Nexus::Graphics::IndexBufferFormat format)
    {
        switch (format)
        {
        case Nexus::Graphics::IndexBufferFormat::UInt16:
            return DXGI_FORMAT_R16_UINT;
        case Nexus::Graphics::IndexBufferFormat::UInt32:
            return DXGI_FORMAT_R32_UINT;
        }
    }

    CommandListD3D12::CommandListD3D12(GraphicsDeviceD3D12 *device)
    {
        auto d3d12Device = device->GetDevice();
        d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator));
        d3d12Device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_CommandList));
    }

    CommandListD3D12::~CommandListD3D12()
    {
    }

    void CommandListD3D12::Begin()
    {
        m_CommandAllocator->Reset();
        m_CommandList->Reset(m_CommandAllocator.Get(), nullptr);
        m_CurrentRenderPass = nullptr;
    }

    void CommandListD3D12::End()
    {
        m_CommandList->Close();
    }

    void CommandListD3D12::BeginRenderPass(RenderPass *renderPass, const RenderPassBeginInfo &beginInfo)
    {
        m_CurrentRenderPass = renderPass;
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> targets;

        if (renderPass->GetRenderPassDataType() == Nexus::Graphics::RenderPassDataType::Swapchain)
        {
            Swapchain *swapchain = renderPass->GetData<Swapchain *>();
            SwapchainD3D12 *d3d12Swapchain = (SwapchainD3D12 *)swapchain;
            targets = {d3d12Swapchain->RetrieveRenderTargetViewDescriptorHandles()[d3d12Swapchain->GetCurrentBufferIndex()]};

            D3D12_RESOURCE_BARRIER renderTargetBarrier;
            renderTargetBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            renderTargetBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            renderTargetBarrier.Transition.pResource = d3d12Swapchain->RetrieveBufferHandles()[d3d12Swapchain->GetCurrentBufferIndex()];
            renderTargetBarrier.Transition.Subresource = 0;
            renderTargetBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
            renderTargetBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
            m_CommandList->ResourceBarrier(1, &renderTargetBarrier);
        }

        if (renderPass->GetColorLoadOperation() == Nexus::Graphics::LoadOperation::Clear)
        {
            float clearColor[] =
                {
                    beginInfo.ClearColorValue.Red,
                    beginInfo.ClearColorValue.Green,
                    beginInfo.ClearColorValue.Blue,
                    beginInfo.ClearColorValue.Alpha};

            for (const auto &target : targets)
            {
                m_CommandList->ClearRenderTargetView(
                    target,
                    clearColor,
                    0,
                    nullptr);
            }
        }

        m_CommandList->OMSetRenderTargets(
            targets.size(),
            targets.data(),
            false,
            nullptr);
    }

    void CommandListD3D12::EndRenderPass()
    {
        if (m_CurrentRenderPass->GetRenderPassDataType() == Nexus::Graphics::RenderPassDataType::Swapchain)
        {
            Swapchain *swapchain = m_CurrentRenderPass->GetData<Swapchain *>();
            SwapchainD3D12 *d3d12Swapchain = (SwapchainD3D12 *)swapchain;

            D3D12_RESOURCE_BARRIER presentBarrier;
            presentBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            presentBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            presentBarrier.Transition.pResource = d3d12Swapchain->RetrieveBufferHandles()[d3d12Swapchain->GetCurrentBufferIndex()];
            presentBarrier.Transition.Subresource = 0;
            presentBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
            presentBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
            m_CommandList->ResourceBarrier(1, &presentBarrier);
        }
    }

    void CommandListD3D12::SetVertexBuffer(VertexBuffer *vertexBuffer)
    {
        VertexBufferD3D12 *d3d12VertexBuffer = (VertexBufferD3D12 *)vertexBuffer;
        auto vertexBufferView = d3d12VertexBuffer->GetVertexBufferView();
        m_CommandList->IASetVertexBuffers(0, 1, &vertexBufferView);
    }

    void CommandListD3D12::SetIndexBuffer(IndexBuffer *indexBuffer)
    {
        IndexBufferD3D12 *d3d12IndexBuffer = (IndexBufferD3D12 *)indexBuffer;
        auto indexBufferView = d3d12IndexBuffer->GetIndexBufferView();
        m_CommandList->IASetIndexBuffer(&indexBufferView);
    }

    void CommandListD3D12::SetPipeline(Pipeline *pipeline)
    {
        PipelineD3D12 *d3d12Pipeline = (PipelineD3D12 *)pipeline;
        m_CommandList->SetPipelineState(d3d12Pipeline->GetPipelineState());
        m_CommandList->SetGraphicsRootSignature(d3d12Pipeline->GetRootSignature());
        m_CommandList->IASetPrimitiveTopology(d3d12Pipeline->GetPrimitiveTopology());

        const D3D12_VIEWPORT &viewport = d3d12Pipeline->GetViewport();
        const RECT &scissorRect = d3d12Pipeline->GetScissorRectangle();
        m_CommandList->RSSetViewports(1, &viewport);
        m_CommandList->RSSetScissorRects(1, &scissorRect);
    }

    void CommandListD3D12::DrawElements(uint32_t start, uint32_t count)
    {
        m_CommandList->DrawInstanced(count, 1, start, 0);
    }

    void CommandListD3D12::DrawIndexed(uint32_t count, uint32_t offset)
    {
        m_CommandList->DrawIndexedInstanced(count, 1, offset, 0, 0);
    }

    void CommandListD3D12::SetResourceSet(ResourceSet *resources)
    {
        ResourceSetD3D12 *d3d12ResourceSet = (ResourceSetD3D12 *)resources;

        /*ID3D12DescriptorHeap *heaps[] =
            {
                d3d12ResourceSet->GetSamplerDescriptorHeap(),
                d3d12ResourceSet->GetTextureDescriptorHeap(),
                d3d12ResourceSet->GetConstantBufferDescriptorHeap()};

        m_CommandList->SetDescriptorHeaps(3, heaps);

        m_CommandList->SetGraphicsRootDescriptorTable(0, d3d12ResourceSet->GetSamplerGPUStartHandle());
        m_CommandList->SetGraphicsRootDescriptorTable(1, d3d12ResourceSet->GetTextureGPUStartHandle());
        m_CommandList->SetGraphicsRootDescriptorTable(2, d3d12ResourceSet->GetConstantBufferGPUStartHandle()); */

        ID3D12DescriptorHeap *heaps[] =
            {
                d3d12ResourceSet->GetSamplerDescriptorHeap(),
                d3d12ResourceSet->GetTextureConstantBufferDescriptorHeap()};

        m_CommandList->SetDescriptorHeaps(2, heaps);

        m_CommandList->SetGraphicsRootDescriptorTable(0, d3d12ResourceSet->GetSamplerGPUStartHandle());
        m_CommandList->SetGraphicsRootDescriptorTable(1, d3d12ResourceSet->GetTextureGPUStartHandle());
        m_CommandList->SetGraphicsRootDescriptorTable(2, d3d12ResourceSet->GetConstantBufferGPUStartHandle());
    }

    ID3D12GraphicsCommandList7 *CommandListD3D12::GetCommandList()
    {
        return m_CommandList.Get();
    }
}