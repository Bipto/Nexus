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
        const auto &description = pipeline->GetPipelineDescription();

        PipelineD3D12 *d3d12Pipeline = (PipelineD3D12 *)pipeline;
        SetRenderTarget(pipeline->GetPipelineDescription().Target);
        m_CommandList->OMSetDepthBounds(description.DepthStencilDescription.MinDepth, description.DepthStencilDescription.MaxDepth);
        m_CommandList->SetPipelineState(d3d12Pipeline->GetPipelineState());
        m_CommandList->SetGraphicsRootSignature(d3d12Pipeline->GetRootSignature());
        m_CommandList->IASetPrimitiveTopology(d3d12Pipeline->GetPrimitiveTopology());
    }

    void CommandListD3D12::DrawElements(uint32_t start, uint32_t count)
    {
        m_CommandList->DrawInstanced(count, 1, start, 0);
    }

    void CommandListD3D12::DrawIndexed(uint32_t count, uint32_t indexStart, uint32_t vertexStart)
    {
        m_CommandList->DrawIndexedInstanced(count, 1, indexStart, vertexStart, 0);
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
        if (d3d12ResourceSet->GetSpecification().Textures.size() > 0)
        {
            m_CommandList->SetGraphicsRootDescriptorTable(descriptorIndex++, d3d12ResourceSet->GetSamplerGPUStartHandle());
            m_CommandList->SetGraphicsRootDescriptorTable(descriptorIndex++, d3d12ResourceSet->GetTextureGPUStartHandle());
        }

        if (d3d12ResourceSet->GetSpecification().UniformBuffers.size() > 0)
        {
            m_CommandList->SetGraphicsRootDescriptorTable(descriptorIndex++, d3d12ResourceSet->GetConstantBufferGPUStartHandle());
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

        m_CommandList->OMSetRenderTargets(
            m_DescriptorHandles.size(),
            m_DescriptorHandles.data(),
            false,
            &m_DepthHandle);

        m_CurrentRenderTarget = target;
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

        auto framebufferTexture = framebufferD3D12->GetColorTextures()[sourceIndex];
        auto swapchainTexture = swapchainD3D12->RetrieveBufferHandles()[swapchainD3D12->GetCurrentBufferIndex()];
        auto format = GetD3D12TextureFormat(Nexus::Graphics::TextureFormat::RGBA8);
        auto framebufferState = framebufferD3D12->GetCurrentColorTextureStates()[sourceIndex];
        auto swapchainState = swapchainD3D12->GetCurrentTextureStates()[swapchainD3D12->GetCurrentBufferIndex()];

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
        framebufferBarrier.Transition.pResource = framebufferTexture.Get();
        framebufferBarrier.Transition.Subresource = 0;
        framebufferBarrier.Transition.StateBefore = framebufferState;
        framebufferBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
        resourceBarriers.push_back(framebufferBarrier);

        D3D12_RESOURCE_BARRIER swapchainBarrier;
        swapchainBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        swapchainBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        swapchainBarrier.Transition.pResource = swapchainTexture;
        swapchainBarrier.Transition.Subresource = 0;
        swapchainBarrier.Transition.StateBefore = swapchainState;
        swapchainBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_DEST;
        resourceBarriers.push_back(swapchainBarrier);

        m_CommandList->ResourceBarrier(
            resourceBarriers.size(),
            resourceBarriers.data());

        m_CommandList->ResolveSubresource(
            swapchainTexture,
            0,
            framebufferTexture.Get(),
            0,
            format);

        framebufferD3D12->SetColorTextureState(D3D12_RESOURCE_STATE_RESOLVE_SOURCE, sourceIndex);
        swapchainD3D12->SetTextureState(D3D12_RESOURCE_STATE_RESOLVE_DEST, swapchainD3D12->GetCurrentBufferIndex());
    }

    ID3D12GraphicsCommandList7 *CommandListD3D12::GetCommandList()
    {
        return m_CommandList.Get();
    }

    void CommandListD3D12::SetSwapchain(SwapchainD3D12 *swapchain)
    {
        ResetPreviousRenderTargets();
        m_DescriptorHandles = {swapchain->RetrieveRenderTargetViewDescriptorHandles()[swapchain->GetCurrentBufferIndex()]};
        m_DepthHandle = swapchain->RetrieveDepthBufferDescriptorHandle();
        auto depthState = swapchain->GetCurrentTextureStates()[swapchain->GetCurrentBufferIndex()];

        D3D12_RESOURCE_BARRIER renderTargetBarrier;
        renderTargetBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        renderTargetBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        renderTargetBarrier.Transition.pResource = swapchain->RetrieveBufferHandles()[swapchain->GetCurrentBufferIndex()];
        renderTargetBarrier.Transition.Subresource = 0;
        renderTargetBarrier.Transition.StateBefore = depthState;
        renderTargetBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        m_CommandList->ResourceBarrier(1, &renderTargetBarrier);

        swapchain->SetTextureState(D3D12_RESOURCE_STATE_RENDER_TARGET, swapchain->GetCurrentBufferIndex());
    }

    void CommandListD3D12::SetFramebuffer(FramebufferD3D12 *framebuffer)
    {
        ResetPreviousRenderTargets();

        std::vector<D3D12_RESOURCE_BARRIER> resourceBarriers;
        m_DescriptorHandles = framebuffer->GetColorAttachmentHandles();
        auto colorTextures = framebuffer->GetColorTextures();

        for (int i = 0; i < m_DescriptorHandles.size(); i++)
        {
            auto resourceState = framebuffer->GetCurrentColorTextureStates()[i];

            D3D12_RESOURCE_BARRIER renderTargetBarrier;
            renderTargetBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            renderTargetBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            renderTargetBarrier.Transition.pResource = colorTextures[i].Get();
            renderTargetBarrier.Transition.Subresource = 0;
            renderTargetBarrier.Transition.StateBefore = resourceState;
            renderTargetBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
            resourceBarriers.push_back(renderTargetBarrier);

            framebuffer->SetColorTextureState(D3D12_RESOURCE_STATE_RENDER_TARGET, 0);
        }

        if (framebuffer->HasDepthTexture())
        {
            m_DepthHandle = framebuffer->GetDepthAttachmentHandle();
            auto depthTexture = framebuffer->GetDepthTexture();
            auto depthState = framebuffer->GetCurrentDepthState();

            D3D12_RESOURCE_BARRIER depthTargetBarrier;
            depthTargetBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            depthTargetBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            depthTargetBarrier.Transition.pResource = depthTexture.Get();
            depthTargetBarrier.Transition.Subresource = 0;
            depthTargetBarrier.Transition.StateBefore = depthState;
            depthTargetBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
            resourceBarriers.push_back(depthTargetBarrier);

            framebuffer->SetDepthState(D3D12_RESOURCE_STATE_DEPTH_WRITE);
        }
        else
        {
            m_DepthHandle = {};
        }

        m_CommandList->ResourceBarrier(resourceBarriers.size(), resourceBarriers.data());
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
            D3D12_RESOURCE_BARRIER presentBarrier;
            presentBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            presentBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            presentBarrier.Transition.pResource = d3d12Swapchain->RetrieveBufferHandles()[d3d12Swapchain->GetCurrentBufferIndex()];
            presentBarrier.Transition.Subresource = 0;
            presentBarrier.Transition.StateBefore = d3d12Swapchain->GetCurrentTextureStates()[d3d12Swapchain->GetCurrentBufferIndex()];
            presentBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
            m_CommandList->ResourceBarrier(1, &presentBarrier);

            d3d12Swapchain->SetTextureState(D3D12_RESOURCE_STATE_PRESENT, d3d12Swapchain->GetCurrentBufferIndex());
        }
        else if (m_CurrentRenderTarget.GetType() == RenderTargetType::Framebuffer)
        {
            auto d3d12Framebuffer = (FramebufferD3D12 *)m_CurrentRenderTarget.GetData<Framebuffer *>();
            std::vector<D3D12_RESOURCE_BARRIER> resourceBarriers;
            auto colorTextures = d3d12Framebuffer->GetColorTextures();

            for (int i = 0; i < d3d12Framebuffer->GetColorTextureCount(); i++)
            {
                auto textureState = d3d12Framebuffer->GetCurrentColorTextureStates()[i];

                D3D12_RESOURCE_BARRIER renderTargetBarrier;
                renderTargetBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                renderTargetBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                renderTargetBarrier.Transition.pResource = colorTextures[i].Get();
                renderTargetBarrier.Transition.Subresource = 0;
                renderTargetBarrier.Transition.StateBefore = textureState;
                renderTargetBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
                resourceBarriers.push_back(renderTargetBarrier);

                d3d12Framebuffer->SetColorTextureState(D3D12_RESOURCE_STATE_COMMON, i);
            }

            if (d3d12Framebuffer->HasDepthTexture())
            {
                auto depthTexture = d3d12Framebuffer->GetDepthTexture();
                auto depthState = d3d12Framebuffer->GetCurrentDepthState();

                D3D12_RESOURCE_BARRIER depthTargetBarrier;
                depthTargetBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                depthTargetBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                depthTargetBarrier.Transition.pResource = depthTexture.Get();
                depthTargetBarrier.Transition.Subresource = 0;
                depthTargetBarrier.Transition.StateBefore = depthState;
                depthTargetBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_READ;
                resourceBarriers.push_back(depthTargetBarrier);

                d3d12Framebuffer->SetDepthState(D3D12_RESOURCE_STATE_DEPTH_READ);
            }
            m_CommandList->ResourceBarrier(resourceBarriers.size(), resourceBarriers.data());
        }

        m_CurrentRenderTarget = {};
        m_DepthHandle = {};
    }
}

#endif