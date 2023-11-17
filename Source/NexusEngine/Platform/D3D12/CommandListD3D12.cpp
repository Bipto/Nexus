#include "CommandListD3D12.hpp"

#include "SwapchainD3D12.hpp"
#include "PipelineD3D12.hpp"
#include "BufferD3D12.hpp"
#include "ResourceSetD3D12.hpp"
#include "FramebufferD3D12.hpp"
#include "RenderPassD3D12.hpp"

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
        D3D12_CPU_DESCRIPTOR_HANDLE *depthHandle = nullptr;

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
        else
        {
            std::vector<D3D12_RESOURCE_BARRIER> resourceBarriers;

            RenderPassD3D12 *d3d12RenderPass = (RenderPassD3D12 *)renderPass;
            FramebufferD3D12 *framebuffer = (FramebufferD3D12 *)d3d12RenderPass->GetFramebuffer();
            targets = framebuffer->GetColorAttachmentHandles();
            auto colorTextures = framebuffer->GetColorTextures();

            for (int i = 0; i < targets.size(); i++)
            {
                D3D12_RESOURCE_BARRIER renderTargetBarrier;
                renderTargetBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                renderTargetBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                renderTargetBarrier.Transition.pResource = colorTextures[i].Get();
                renderTargetBarrier.Transition.Subresource = 0;
                renderTargetBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
                renderTargetBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
                resourceBarriers.push_back(renderTargetBarrier);
            }

            if (framebuffer->HasDepthTexture())
            {
                depthHandle = &framebuffer->GetDepthAttachmentHandle();
                auto depthTexture = framebuffer->GetDepthTexture();

                D3D12_RESOURCE_BARRIER depthTargetBarrier;
                depthTargetBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                depthTargetBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                depthTargetBarrier.Transition.pResource = depthTexture.Get();
                depthTargetBarrier.Transition.Subresource = 0;
                depthTargetBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
                depthTargetBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
                resourceBarriers.push_back(depthTargetBarrier);
            }

            m_CommandList->ResourceBarrier(resourceBarriers.size(), resourceBarriers.data());
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
            depthHandle);
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
        else
        {
            std::vector<D3D12_RESOURCE_BARRIER> resourceBarriers;

            RenderPassD3D12 *d3d12RenderPass = (RenderPassD3D12 *)m_CurrentRenderPass;
            FramebufferD3D12 *framebuffer = (FramebufferD3D12 *)d3d12RenderPass->GetFramebuffer();
            auto colorTextures = framebuffer->GetColorTextures();

            for (int i = 0; i < framebuffer->GetColorTextureCount(); i++)
            {
                D3D12_RESOURCE_BARRIER renderTargetBarrier;
                renderTargetBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                renderTargetBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                renderTargetBarrier.Transition.pResource = colorTextures[i].Get();
                renderTargetBarrier.Transition.Subresource = 0;
                renderTargetBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
                renderTargetBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
                resourceBarriers.push_back(renderTargetBarrier);
            }

            if (framebuffer->HasDepthTexture())
            {
                auto depthTexture = framebuffer->GetDepthTexture();

                D3D12_RESOURCE_BARRIER depthTargetBarrier;
                depthTargetBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                depthTargetBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                depthTargetBarrier.Transition.pResource = depthTexture.Get();
                depthTargetBarrier.Transition.Subresource = 0;
                depthTargetBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
                depthTargetBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
                resourceBarriers.push_back(depthTargetBarrier);
            }

            m_CommandList->ResourceBarrier(resourceBarriers.size(), resourceBarriers.data());
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

        std::vector<ID3D12DescriptorHeap *> heaps;
        if (d3d12ResourceSet->GetSpecification().TextureBindings.size() > 0)
        {
            heaps.push_back(d3d12ResourceSet->GetSamplerDescriptorHeap());
        }

        uint32_t textureConstantBufferDescriptorCount = d3d12ResourceSet->GetSpecification().TextureBindings.size() + d3d12ResourceSet->GetSpecification().UniformResourceBindings.size();

        if (textureConstantBufferDescriptorCount > 0)
        {
            heaps.push_back(d3d12ResourceSet->GetTextureConstantBufferDescriptorHeap());
        }

        m_CommandList->SetDescriptorHeaps(heaps.size(), heaps.data());

        uint32_t descriptorIndex = 0;
        if (d3d12ResourceSet->GetSpecification().TextureBindings.size() > 0)
        {
            m_CommandList->SetGraphicsRootDescriptorTable(descriptorIndex, d3d12ResourceSet->GetSamplerGPUStartHandle());
            descriptorIndex++;
            m_CommandList->SetGraphicsRootDescriptorTable(descriptorIndex, d3d12ResourceSet->GetTextureGPUStartHandle());
            descriptorIndex++;
        }

        if (d3d12ResourceSet->GetSpecification().UniformResourceBindings.size() > 0)
        {
            m_CommandList->SetGraphicsRootDescriptorTable(descriptorIndex, d3d12ResourceSet->GetConstantBufferGPUStartHandle());
            descriptorIndex++;
        }
    }

    ID3D12GraphicsCommandList7 *CommandListD3D12::GetCommandList()
    {
        return m_CommandList.Get();
    }
}