#if defined(NX_PLATFORM_D3D12)

#include "CommandExecutorD3D12.hpp"

#include "BufferD3D12.hpp"
#include "FramebufferD3D12.hpp"
#include "PipelineD3D12.hpp"
#include "ResourceSetD3D12.hpp"
#include "SwapchainD3D12.hpp"
#include "TextureD3D12.hpp"
#include "TimingQueryD3D12.hpp"

namespace Nexus::Graphics
{
CommandExecutorD3D12::CommandExecutorD3D12()
{
}

CommandExecutorD3D12::~CommandExecutorD3D12()
{
}

void CommandExecutorD3D12::ExecuteCommands(const std::vector<RenderCommandData> &commands, GraphicsDevice *device)
{
    for (const auto &element : commands)
    {
        std::visit([&](auto &&arg) { ExecuteCommand(arg, device); }, element);
    }
}

void CommandExecutorD3D12::Reset()
{
}

void CommandExecutorD3D12::SetCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList)
{
    m_CommandList = commandList;
}

void CommandExecutorD3D12::ExecuteCommand(SetVertexBufferCommand command, GraphicsDevice *device)
{
    Ref<PipelineD3D12> pipeline = m_CurrentlyBoundPipeline.lock();
    Ref<VertexBufferD3D12> d3d12VertexBuffer = std::dynamic_pointer_cast<VertexBufferD3D12>(command.VertexBufferRef.lock());
    const auto &bufferLayout = pipeline->GetPipelineDescription().Layouts.at(command.Slot);

    D3D12_VERTEX_BUFFER_VIEW bufferView;
    bufferView.BufferLocation = d3d12VertexBuffer->GetHandle()->GetGPUVirtualAddress();
    bufferView.SizeInBytes = d3d12VertexBuffer->GetDescription().Size;
    bufferView.StrideInBytes = bufferLayout.GetStride();

    m_CommandList->IASetVertexBuffers(command.Slot, 1, &bufferView);
}

void CommandExecutorD3D12::ExecuteCommand(WeakRef<IndexBuffer> command, GraphicsDevice *device)
{
    Ref<IndexBufferD3D12> d3d12IndexBuffer = std::dynamic_pointer_cast<IndexBufferD3D12>(command.lock());
    auto indexBufferView = d3d12IndexBuffer->GetIndexBufferView();
    m_CommandList->IASetIndexBuffer(&indexBufferView);
}

void CommandExecutorD3D12::ExecuteCommand(WeakRef<Pipeline> command, GraphicsDevice *device)
{
    Ref<PipelineD3D12> d3d12Pipeline = std::dynamic_pointer_cast<PipelineD3D12>(command.lock());
    const auto &description = d3d12Pipeline->GetPipelineDescription();

    ExecuteCommand(d3d12Pipeline->GetPipelineDescription().Target, device);

    m_CommandList->OMSetDepthBounds(description.DepthStencilDesc.MinDepth, description.DepthStencilDesc.MaxDepth);
    m_CommandList->SetPipelineState(d3d12Pipeline->GetPipelineState());
    m_CommandList->SetGraphicsRootSignature(d3d12Pipeline->GetRootSignature());
    m_CommandList->IASetPrimitiveTopology(d3d12Pipeline->GetD3DPrimitiveTopology());

    m_CurrentlyBoundPipeline = d3d12Pipeline;
}

void CommandExecutorD3D12::ExecuteCommand(DrawElementCommand command, GraphicsDevice *device)
{
    m_CommandList->DrawInstanced(command.Count, 1, command.Start, 0);
}

void CommandExecutorD3D12::ExecuteCommand(DrawIndexedCommand command, GraphicsDevice *device)
{
    m_CommandList->DrawIndexedInstanced(command.Count, 1, command.IndexStart, command.VertexStart, 0);
}

void CommandExecutorD3D12::ExecuteCommand(DrawInstancedCommand command, GraphicsDevice *device)
{
    m_CommandList->DrawInstanced(command.VertexCount, command.InstanceCount, command.VertexStart, command.InstanceStart);
}

void CommandExecutorD3D12::ExecuteCommand(DrawInstancedIndexedCommand command, GraphicsDevice *device)
{
    m_CommandList->DrawIndexedInstanced(command.IndexCount, command.InstanceCount, command.IndexStart, command.VertexStart, command.InstanceStart);
}

void CommandExecutorD3D12::ExecuteCommand(Ref<ResourceSet> command, GraphicsDevice *device)
{
    Ref<ResourceSetD3D12> d3d12ResourceSet = std::dynamic_pointer_cast<ResourceSetD3D12>(command);

    if (m_CommandListSpecification.AutomaticLayoutManagement)
    {
        for (const auto &[name, combinedImageSampler] : d3d12ResourceSet->GetBoundCombinedImageSamplers())
        {
            if (combinedImageSampler.ImageTexture.expired())
            {
                NX_ERROR("Attempting to bind an invalid texture");
                continue;
            }

            Ref<Texture> texture = combinedImageSampler.ImageTexture.lock();

            TransitionImageLayoutCommand transition;
            transition.TransitionTexture = combinedImageSampler.ImageTexture;
            transition.TextureLayout = Nexus::Graphics::ImageLayout::ShaderRead;
            transition.BaseLevel = 0;
            transition.NumLevels = texture->GetLevels();
            ExecuteCommand(transition, device);
        }
    }

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

void CommandExecutorD3D12::ExecuteCommand(ClearColorTargetCommand command, GraphicsDevice *device)
{
    if (m_DescriptorHandles.size() > command.Index)
    {
        float clearColor[] = {command.Color.Red, command.Color.Green, command.Color.Blue, command.Color.Alpha};

        auto target = m_DescriptorHandles[command.Index];
        m_CommandList->ClearRenderTargetView(target, clearColor, 0, nullptr);
    }
}

void CommandExecutorD3D12::ExecuteCommand(ClearDepthStencilTargetCommand command, GraphicsDevice *device)
{
    if (m_DepthHandle.ptr)
    {
        D3D12_CLEAR_FLAGS clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;

        m_CommandList->ClearDepthStencilView(m_DepthHandle, clearFlags, command.Value.Depth, command.Value.Stencil, 0, nullptr);
    }
}

void CommandExecutorD3D12::ExecuteCommand(RenderTarget command, GraphicsDevice *device)
{
    if (command.GetType() == RenderTargetType::Swapchain)
    {
        auto d3d12Swapchain = (SwapchainD3D12 *)command.GetData<Swapchain *>();
        SetSwapchain(d3d12Swapchain, device);
    }
    else if (command.GetType() == RenderTargetType::Framebuffer)
    {
        auto d3d12Framebuffer = std::dynamic_pointer_cast<FramebufferD3D12>(command.GetData<Ref<Framebuffer>>());
        SetFramebuffer(d3d12Framebuffer, device);
    }
    else
    {
        throw std::runtime_error("Invalid render target type selected");
    }

    m_CurrentRenderTarget = command;

    if (m_CurrentRenderTarget.HasDepthAttachment())
    {
        m_CommandList->OMSetRenderTargets(m_DescriptorHandles.size(), m_DescriptorHandles.data(), false, &m_DepthHandle);
    }
    else
    {
        m_CommandList->OMSetRenderTargets(m_DescriptorHandles.size(), m_DescriptorHandles.data(), false, nullptr);
    }
}

void CommandExecutorD3D12::ExecuteCommand(const Viewport &command, GraphicsDevice *device)
{
    D3D12_VIEWPORT vp;
    vp.TopLeftX = command.X;
    vp.TopLeftY = command.Y;
    vp.Width = command.Width;
    vp.Height = command.Height;
    vp.MinDepth = command.MinDepth;
    vp.MaxDepth = command.MaxDepth;
    m_CommandList->RSSetViewports(1, &vp);
}

void CommandExecutorD3D12::ExecuteCommand(const Scissor &command, GraphicsDevice *device)
{
    RECT rect;
    rect.left = command.X;
    rect.top = command.Y;
    rect.right = command.Width + command.X;
    rect.bottom = command.Height + command.Y;
    m_CommandList->RSSetScissorRects(1, &rect);
}

void CommandExecutorD3D12::ExecuteCommand(ResolveSamplesToSwapchainCommand command, GraphicsDevice *device)
{
    auto framebufferD3D12 = std::dynamic_pointer_cast<FramebufferD3D12>(command.Source.lock());
    auto swapchainD3D12 = (SwapchainD3D12 *)command.Target;

    if (command.SourceIndex > framebufferD3D12->GetColorTextureCount())
    {
        return;
    }

    Nexus::Ref<Nexus::Graphics::TextureD3D12> framebufferTexture = framebufferD3D12->GetD3D12ColorTexture(command.SourceIndex);
    Microsoft::WRL::ComPtr<ID3D12Resource2> swapchainTexture = swapchainD3D12->RetrieveBufferHandle();
    DXGI_FORMAT format = D3D12::GetD3D12PixelFormat(Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm, false);
    D3D12_RESOURCE_STATES swapchainState = swapchainD3D12->GetCurrentTextureState();

    if (framebufferD3D12->GetFramebufferSpecification().Width > swapchainD3D12->GetWindow()->GetWindowSize().X)
    {
        return;
    }

    if (framebufferD3D12->GetFramebufferSpecification().Height > swapchainD3D12->GetWindow()->GetWindowSize().Y)
    {
        return;
    }

    TransitionImageLayoutCommand transition;
    transition.TransitionTexture = framebufferTexture;
    transition.TextureLayout = Nexus::Graphics::ImageLayout::ResolveSource;
    transition.BaseLevel = 0;
    transition.NumLevels = 1;
    ExecuteCommand(transition, device);

    D3D12_RESOURCE_BARRIER swapchainBarrier;
    swapchainBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    swapchainBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    swapchainBarrier.Transition.pResource = swapchainTexture.Get();
    swapchainBarrier.Transition.Subresource = 0;
    swapchainBarrier.Transition.StateBefore = swapchainState;
    swapchainBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_DEST;

    m_CommandList->ResourceBarrier(1, &swapchainBarrier);

    m_CommandList->ResolveSubresource(swapchainTexture.Get(), 0, framebufferTexture->GetD3D12ResourceHandle().Get(), 0, format);

    swapchainD3D12->SetTextureState(D3D12_RESOURCE_STATE_RESOLVE_DEST);
}

void CommandExecutorD3D12::ExecuteCommand(StartTimingQueryCommand command, GraphicsDevice *device)
{
    Ref<TimingQueryD3D12> queryD3D12 = std::dynamic_pointer_cast<TimingQueryD3D12>(command.Query.lock());
    Microsoft::WRL::ComPtr<ID3D12QueryHeap> heap = queryD3D12->GetQueryHeap();

    m_CommandList->EndQuery(heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 0);
}

void CommandExecutorD3D12::ExecuteCommand(StopTimingQueryCommand command, GraphicsDevice *device)
{
    Ref<TimingQueryD3D12> queryD3D12 = std::dynamic_pointer_cast<TimingQueryD3D12>(command.Query.lock());
    Microsoft::WRL::ComPtr<ID3D12QueryHeap> heap = queryD3D12->GetQueryHeap();

    m_CommandList->EndQuery(heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 1);
}

void CommandExecutorD3D12::ExecuteCommand(const TransitionImageLayoutCommand &command, GraphicsDevice *device)
{
    if (command.TransitionTexture.expired())
    {
        NX_ERROR("Attempting to transition an invalid texture");
        return;
    }

    if (Ref<TextureD3D12> texture = std::dynamic_pointer_cast<TextureD3D12>(command.TransitionTexture.lock()))
    {
        for (uint32_t i = command.BaseLevel; i < command.BaseLevel + command.NumLevels; i++)
        {
            ImageLayout layout = texture->GetImageLayout(i).value();
            D3D12_RESOURCE_STATES beforeState = D3D12::GetD3D12ResourceStatesFromNxImageLayout(layout);
            D3D12_RESOURCE_STATES afterState = D3D12::GetD3D12ResourceStatesFromNxImageLayout(command.TextureLayout);

            if (beforeState != afterState)
            {
                D3D12_RESOURCE_BARRIER barrier{};
                barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                barrier.Transition.pResource = texture->GetD3D12ResourceHandle().Get();
                barrier.Transition.Subresource = i;
                barrier.Transition.StateBefore = beforeState;
                barrier.Transition.StateAfter = afterState;

                m_CommandList->ResourceBarrier(1, &barrier);
            }

            SetImageLayout(texture, i, command.TextureLayout);
        }
    }
}

void CommandExecutorD3D12::SetSwapchain(SwapchainD3D12 *swapchain, GraphicsDevice *device)
{
    if (swapchain->GetSpecification().Samples == SampleCount::SampleCount1)
    {
        std::vector<D3D12_RESOURCE_BARRIER> barriers;

        ResetPreviousRenderTargets(device);
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
        SetFramebuffer(std::dynamic_pointer_cast<FramebufferD3D12>(swapchain->GetMultisampledFramebuffer()), device);
    }
}

void CommandExecutorD3D12::SetFramebuffer(Ref<FramebufferD3D12> framebuffer, GraphicsDevice *device)
{
    ResetPreviousRenderTargets(device);

    m_DescriptorHandles = framebuffer->GetColorAttachmentCPUHandles();

    for (size_t i = 0; i < framebuffer->GetColorTextureCount(); i++)
    {
        Ref<Texture> texture = framebuffer->GetColorTexture(i);

        TransitionImageLayoutCommand transition;
        transition.TransitionTexture = texture;
        transition.TextureLayout = Nexus::Graphics::ImageLayout::Colour;
        transition.BaseLevel = 0;
        transition.NumLevels = texture->GetLevels();
        ExecuteCommand(transition, device);
    }

    if (framebuffer->HasDepthTexture())
    {
        m_DepthHandle = framebuffer->GetDepthAttachmentCPUHandle();

        Ref<Texture> depthBuffer = framebuffer->GetDepthTexture();

        TransitionImageLayoutCommand transition;
        transition.TransitionTexture = depthBuffer;
        transition.TextureLayout = Nexus::Graphics::ImageLayout::DepthReadWrite;
        transition.BaseLevel = 0;
        transition.NumLevels = 1;
        ExecuteCommand(transition, device);
    }
    else
    {
        m_DepthHandle = {};
    }
}

void CommandExecutorD3D12::ResetPreviousRenderTargets(GraphicsDevice *device)
{
    if (m_CurrentRenderTarget.GetType() == RenderTargetType::None)
    {
        return;
    }

    if (m_CurrentRenderTarget.GetType() == RenderTargetType::Swapchain)
    {
        auto d3d12Swapchain = (SwapchainD3D12 *)m_CurrentRenderTarget.GetData<Swapchain *>();

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

    m_CurrentRenderTarget = {};
    m_DepthHandle = {};
}
} // namespace Nexus::Graphics

#endif