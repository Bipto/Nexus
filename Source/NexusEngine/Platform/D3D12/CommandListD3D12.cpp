#include "CommandListD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

#include "SwapchainD3D12.hpp"
#include "PipelineD3D12.hpp"
#include "BufferD3D12.hpp"
#include "ResourceSetD3D12.hpp"
#include "FramebufferD3D12.hpp"
#include "D3D12Utils.hpp"

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

        m_CurrentRenderTarget = {};
    }

    void CommandListD3D12::End()
    {
        ResetPreviousRenderTargets();
        m_CommandList->Close();
    }

    void CommandListD3D12::SetVertexBuffer(VertexBuffer *vertexBuffer, uint32_t slot)
    {
        VertexBufferD3D12 *d3d12VertexBuffer = (VertexBufferD3D12 *)vertexBuffer;
        auto vertexBufferView = d3d12VertexBuffer->GetVertexBufferView();
        m_CommandList->IASetVertexBuffers(slot, 1, &vertexBufferView);
    }

    void CommandListD3D12::SetIndexBuffer(IndexBuffer *indexBuffer)
    {
        IndexBufferD3D12 *d3d12IndexBuffer = (IndexBufferD3D12 *)indexBuffer;
        auto indexBufferView = d3d12IndexBuffer->GetIndexBufferView();
        m_CommandList->IASetIndexBuffer(&indexBufferView);
    }

    void CommandListD3D12::SetPipeline(Pipeline *pipeline)
    {
        const auto &description = pipeline->GetPipelineDescription();

        PipelineD3D12 *d3d12Pipeline = (PipelineD3D12 *)pipeline;
        SetRenderTarget(pipeline->GetPipelineDescription().Target);
        m_CommandList->OMSetDepthBounds(description.DepthStencilDescription.MinDepth, description.DepthStencilDescription.MaxDepth);
        m_CommandList->SetPipelineState(d3d12Pipeline->GetPipelineState());
        m_CommandList->SetGraphicsRootSignature(d3d12Pipeline->GetRootSignature());
        m_CommandList->IASetPrimitiveTopology(d3d12Pipeline->GetPrimitiveTopology());
    }

    void CommandListD3D12::Draw(uint32_t start, uint32_t count)
    {
        m_CommandList->DrawInstanced(count, 1, start, 0);
    }

    void CommandListD3D12::DrawIndexed(uint32_t count, uint32_t indexStart, uint32_t vertexStart)
    {
        m_CommandList->DrawIndexedInstanced(count, 1, indexStart, vertexStart, 0);
    }

    void CommandListD3D12::DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t instanceStart)
    {
        m_CommandList->DrawInstanced(vertexCount, instanceCount, vertexStart, instanceStart);
    }

    void CommandListD3D12::DrawInstancedIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t indexStart, uint32_t instanceStart)
    {
        m_CommandList->DrawIndexedInstanced(indexCount, instanceCount, indexStart, vertexStart, instanceStart);
    }

    void CommandListD3D12::SetResourceSet(ResourceSet *resources)
    {
        ResourceSetD3D12 *d3d12ResourceSet = (ResourceSetD3D12 *)resources;

        std::vector<ID3D12DescriptorHeap *> heaps;
        if (d3d12ResourceSet->HasSamplerHeap())
        {
            heaps.push_back(d3d12ResourceSet->GetSamplerDescriptorHeap());
        }

        if (d3d12ResourceSet->HasConstantBufferTextureHeap())
        {
            heaps.push_back(d3d12ResourceSet->GetTextureConstantBufferDescriptorHeap());
        }

        m_CommandList->SetDescriptorHeaps(heaps.size(), heaps.data());

        uint32_t descriptorIndex = 0;

        // bind samplers
        {
            m_CommandList->SetGraphicsRootDescriptorTable(descriptorIndex++, d3d12ResourceSet->GetSamplerStartHandle());
        }

        // bind textures/constant buffers
        {
            m_CommandList->SetGraphicsRootDescriptorTable(descriptorIndex++, d3d12ResourceSet->GetTextureConstantBufferStartHandle());
        }
    }

    void CommandListD3D12::ClearColorTarget(uint32_t index, const ClearColorValue &color)
    {
        if (m_DescriptorHandles.size() > index)
        {
            float clearColor[] =
                {
                    color.Red,
                    color.Green,
                    color.Blue,
                    color.Alpha};

            auto target = m_DescriptorHandles[index];
            m_CommandList->ClearRenderTargetView(
                target,
                clearColor,
                0,
                nullptr);
        }
    }

    void CommandListD3D12::ClearDepthTarget(const ClearDepthStencilValue &value)
    {
        if (m_DepthHandle.ptr)
        {
            D3D12_CLEAR_FLAGS clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;

            m_CommandList->ClearDepthStencilView(
                m_DepthHandle,
                clearFlags,
                value.Depth,
                value.Stencil,
                0,
                nullptr);
        }
    }

    void CommandListD3D12::SetRenderTarget(RenderTarget target)
    {
        if (target.GetType() == RenderTargetType::Swapchain)
        {
            auto d3d12Swapchain = (SwapchainD3D12 *)target.GetData<Swapchain *>();
            SetSwapchain(d3d12Swapchain);
        }
        else if (target.GetType() == RenderTargetType::Framebuffer)
        {
            auto d3d12Framebuffer = (FramebufferD3D12 *)target.GetData<Framebuffer *>();
            SetFramebuffer(d3d12Framebuffer);
        }
        else
        {
            throw std::runtime_error("Invalid render target type selected");
        }

        m_CurrentRenderTarget = target;

        if (m_CurrentRenderTarget.HasDepthAttachment())
        {
            m_CommandList->OMSetRenderTargets(
                m_DescriptorHandles.size(),
                m_DescriptorHandles.data(),
                false,
                &m_DepthHandle);
        }
        else
        {
            m_CommandList->OMSetRenderTargets(
                m_DescriptorHandles.size(),
                m_DescriptorHandles.data(),
                false,
                nullptr);
        }
    }

    void CommandListD3D12::SetViewport(const Viewport &viewport)
    {
        D3D12_VIEWPORT vp;
        vp.TopLeftX = viewport.X;
        vp.TopLeftY = viewport.Y;
        vp.Width = viewport.Width;
        vp.Height = viewport.Height;
        vp.MinDepth = viewport.MinDepth;
        vp.MaxDepth = viewport.MaxDepth;
        m_CommandList->RSSetViewports(1, &vp);
    }

    void CommandListD3D12::SetScissor(const Scissor &scissor)
    {
        RECT rect;
        rect.left = scissor.X;
        rect.top = scissor.Y;
        rect.right = scissor.Width + scissor.X;
        rect.bottom = scissor.Height + scissor.Y;
        m_CommandList->RSSetScissorRects(1, &rect);
    }

    void CommandListD3D12::ResolveFramebuffer(Framebuffer *source, uint32_t sourceIndex, Swapchain *target)
    {
        auto framebufferD3D12 = (FramebufferD3D12 *)source;
        auto swapchainD3D12 = (SwapchainD3D12 *)target;

        if (sourceIndex > framebufferD3D12->GetColorTextureCount())
        {
            return;
        }

        auto framebufferTexture = framebufferD3D12->GetD3D12ColorTexture(sourceIndex);
        auto swapchainTexture = swapchainD3D12->RetrieveBufferHandle();
        auto format = GetD3D12PixelFormat(Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm, false);
        auto framebufferState = framebufferTexture->GetCurrentResourceState();
        auto swapchainState = swapchainD3D12->GetCurrentTextureState();

        if (framebufferD3D12->GetFramebufferSpecification().Width > swapchainD3D12->GetWindow()->GetWindowSize().X)
        {
            return;
        }

        if (framebufferD3D12->GetFramebufferSpecification().Height > swapchainD3D12->GetWindow()->GetWindowSize().Y)
        {
            return;
        }

        std::vector<D3D12_RESOURCE_BARRIER> resourceBarriers;

        // framebuffer texture
        D3D12_RESOURCE_BARRIER framebufferBarrier;
        framebufferBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        framebufferBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        framebufferBarrier.Transition.pResource = framebufferTexture->GetD3D12ResourceHandle().Get();
        framebufferBarrier.Transition.Subresource = 0;
        framebufferBarrier.Transition.StateBefore = framebufferState;
        framebufferBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
        resourceBarriers.push_back(framebufferBarrier);

        D3D12_RESOURCE_BARRIER swapchainBarrier;
        swapchainBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        swapchainBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        swapchainBarrier.Transition.pResource = swapchainTexture.Get();
        swapchainBarrier.Transition.Subresource = 0;
        swapchainBarrier.Transition.StateBefore = swapchainState;
        swapchainBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_DEST;
        resourceBarriers.push_back(swapchainBarrier);

        m_CommandList->ResourceBarrier(
            resourceBarriers.size(),
            resourceBarriers.data());

        m_CommandList->ResolveSubresource(
            swapchainTexture.Get(),
            0,
            framebufferTexture->GetD3D12ResourceHandle().Get(),
            0,
            format);

        framebufferTexture->SetCurrentResourceState(D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
        swapchainD3D12->SetTextureState(D3D12_RESOURCE_STATE_RESOLVE_DEST);
    }

    ID3D12GraphicsCommandList7 *CommandListD3D12::GetCommandList()
    {
        return m_CommandList.Get();
    }

    void CommandListD3D12::SetSwapchain(SwapchainD3D12 *swapchain)
    {
        if (swapchain->GetSpecification().Samples == SampleCount::SampleCount1)
        {
            std::vector<D3D12_RESOURCE_BARRIER> barriers;

            ResetPreviousRenderTargets();
            m_DescriptorHandles = {swapchain->RetrieveRenderTargetViewDescriptorHandle()};
            m_DepthHandle = swapchain->RetrieveDepthBufferDescriptorHandle();
            auto colorState = swapchain->GetCurrentTextureState();

            if (colorState != D3D12_RESOURCE_STATE_RENDER_TARGET)
            {
                D3D12_RESOURCE_BARRIER renderTargetBarrier;
                renderTargetBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                renderTargetBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                renderTargetBarrier.Transition.pResource = swapchain->RetrieveBufferHandle().Get();
                renderTargetBarrier.Transition.Subresource = 0;
                renderTargetBarrier.Transition.StateBefore = colorState;
                renderTargetBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
                barriers.push_back(renderTargetBarrier);

                swapchain->SetTextureState(D3D12_RESOURCE_STATE_RENDER_TARGET);
            }

            auto depthState = swapchain->GetCurrentDepthState();

            if (depthState != D3D12_RESOURCE_STATE_DEPTH_WRITE)
            {
                D3D12_RESOURCE_BARRIER depthBarrier;
                depthBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                depthBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                depthBarrier.Transition.pResource = swapchain->RetrieveDepthBufferHandle();
                depthBarrier.Transition.Subresource = 0;
                depthBarrier.Transition.StateBefore = depthState;
                depthBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
                barriers.push_back(depthBarrier);

                swapchain->SetDepthState(D3D12_RESOURCE_STATE_DEPTH_WRITE);
            }

            m_CommandList->ResourceBarrier(barriers.size(), barriers.data());
        }
        else
        {
            SetFramebuffer((FramebufferD3D12 *)swapchain->GetMultisampledFramebuffer());
        }
    }

    void CommandListD3D12::SetFramebuffer(FramebufferD3D12 *framebuffer)
    {
        ResetPreviousRenderTargets();

        std::vector<D3D12_RESOURCE_BARRIER> resourceBarriers;
        m_DescriptorHandles = framebuffer->GetColorAttachmentCPUHandles();

        for (int i = 0; i < m_DescriptorHandles.size(); i++)
        {
            auto texture = framebuffer->GetD3D12ColorTexture(i);

            if (texture->GetCurrentResourceState() != D3D12_RESOURCE_STATE_RENDER_TARGET)
            {
                D3D12_RESOURCE_BARRIER renderTargetBarrier;
                renderTargetBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                renderTargetBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                renderTargetBarrier.Transition.pResource = texture->GetD3D12ResourceHandle().Get();
                renderTargetBarrier.Transition.Subresource = 0;
                renderTargetBarrier.Transition.StateBefore = texture->GetCurrentResourceState();
                renderTargetBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
                resourceBarriers.push_back(renderTargetBarrier);

                texture->SetCurrentResourceState(D3D12_RESOURCE_STATE_RENDER_TARGET);
            }
        }

        if (framebuffer->HasDepthTexture())
        {
            m_DepthHandle = framebuffer->GetDepthAttachmentCPUHandle();
            auto depthTexture = framebuffer->GetD3D12DepthTexture();
            auto resourceHandle = depthTexture->GetD3D12ResourceHandle();

            if (depthTexture->GetCurrentResourceState() != D3D12_RESOURCE_STATE_DEPTH_WRITE)
            {
                D3D12_RESOURCE_BARRIER depthTargetBarrier;
                depthTargetBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                depthTargetBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                depthTargetBarrier.Transition.pResource = resourceHandle.Get();
                depthTargetBarrier.Transition.Subresource = 0;
                depthTargetBarrier.Transition.StateBefore = depthTexture->GetCurrentResourceState();
                depthTargetBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
                resourceBarriers.push_back(depthTargetBarrier);

                depthTexture->SetCurrentResourceState(D3D12_RESOURCE_STATE_DEPTH_WRITE);
            }
        }
        else
        {
            m_DepthHandle = {};
        }

        if (resourceBarriers.size() > 0)
        {
            m_CommandList->ResourceBarrier(resourceBarriers.size(), resourceBarriers.data());
        }
    }

    void CommandListD3D12::ResetPreviousRenderTargets()
    {
        if (m_CurrentRenderTarget.GetType() == RenderTargetType::None)
        {
            return;
        }

        if (m_CurrentRenderTarget.GetType() == RenderTargetType::Swapchain)
        {
            auto d3d12Swapchain = (SwapchainD3D12 *)m_CurrentRenderTarget.GetData<Swapchain *>();

            if (d3d12Swapchain->GetSpecification().Samples != SampleCount::SampleCount1)
            {
                this->ResolveFramebuffer(d3d12Swapchain->GetMultisampledFramebuffer(), 0, d3d12Swapchain);
            }

            auto swapchainColourState = d3d12Swapchain->GetCurrentTextureState();
            if (swapchainColourState != D3D12_RESOURCE_STATE_PRESENT)
            {
                D3D12_RESOURCE_BARRIER presentBarrier;
                presentBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                presentBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                presentBarrier.Transition.pResource = d3d12Swapchain->RetrieveBufferHandle().Get();
                presentBarrier.Transition.Subresource = 0;
                presentBarrier.Transition.StateBefore = swapchainColourState;
                presentBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
                m_CommandList->ResourceBarrier(1, &presentBarrier);

                d3d12Swapchain->SetTextureState(D3D12_RESOURCE_STATE_PRESENT);
            }
        }
        else if (m_CurrentRenderTarget.GetType() == RenderTargetType::Framebuffer)
        {
            auto d3d12Framebuffer = (FramebufferD3D12 *)m_CurrentRenderTarget.GetData<Framebuffer *>();
            std::vector<D3D12_RESOURCE_BARRIER> resourceBarriers;

            for (uint32_t i = 0; i < d3d12Framebuffer->GetColorTextureCount(); i++)
            {
                auto texture = d3d12Framebuffer->GetD3D12ColorTexture(i);

                if (texture->GetCurrentResourceState() != D3D12_RESOURCE_STATE_COMMON)
                {
                    D3D12_RESOURCE_BARRIER renderTargetBarrier;
                    renderTargetBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                    renderTargetBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                    renderTargetBarrier.Transition.pResource = texture->GetD3D12ResourceHandle().Get();
                    renderTargetBarrier.Transition.Subresource = 0;
                    renderTargetBarrier.Transition.StateBefore = texture->GetCurrentResourceState();
                    renderTargetBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
                    resourceBarriers.push_back(renderTargetBarrier);

                    texture->SetCurrentResourceState(D3D12_RESOURCE_STATE_COMMON);
                }
            }

            if (d3d12Framebuffer->HasDepthTexture())
            {
                auto depthTexture = d3d12Framebuffer->GetD3D12DepthTexture();

                if (depthTexture->GetCurrentResourceState() != D3D12_RESOURCE_STATE_DEPTH_READ)
                {
                    D3D12_RESOURCE_BARRIER depthTargetBarrier;
                    depthTargetBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                    depthTargetBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                    depthTargetBarrier.Transition.pResource = depthTexture->GetD3D12ResourceHandle().Get();
                    depthTargetBarrier.Transition.Subresource = 0;
                    depthTargetBarrier.Transition.StateBefore = depthTexture->GetCurrentResourceState();
                    depthTargetBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_READ;
                    resourceBarriers.push_back(depthTargetBarrier);

                    depthTexture->SetCurrentResourceState(D3D12_RESOURCE_STATE_DEPTH_READ);
                }
            }

            if (resourceBarriers.size() > 0)
            {
                m_CommandList->ResourceBarrier(resourceBarriers.size(), resourceBarriers.data());
            }
        }

        m_CurrentRenderTarget = {};
        m_DepthHandle = {};
    }
}

#endif